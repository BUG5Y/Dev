#include <iostream>
#include <string>
#include <sstream>
#include <ws2tcpip.h>
#include "xhttp.h"
#include <WinSock2.h>
#include <map>

#pragma comment(lib, "ws2_32.lib")

namespace xhttp {

bool parse_url(const std::string& url, std::string& protocol, std::string& host, int& port, std::string& path) {
    std::stringstream ss(url);
    std::string temp;

    std::cerr << "xhttp.cpp" << std::endl;

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


std::string buildRequest(const std::string& path, const std::string& host, const std::string& cookies) {
   
    std::string cookies = "ID={ xID }; cookie2=value2"; // Replace with your actual cookies

    return "GET /" + path + " HTTP/1.1\r\n"
           "Host: " + host + "\r\n"
           "Connection: close\r\n"
           "Cookie: " + cookies + "\r\n"
           "\r\n";
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                       GET  
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Randomize the URL path based on common path
// Randomize the parameter names
// Use cookies to send the content

std::string http_get(const std::string& url) { // , const std::map<std::string, std::string>& queryParameters
        std::string protocol, host, path, cookies;
        int port;
        if (!parse_url(url, protocol, host, port, path)) {
            return "";
        }
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed.\n";
            return "";
        }

        SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return "";
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
        serverAddr.sin_port = htons(port);
        
        // Extracting and printing individual components
        
        std::cerr << "Server Address Family: " << serverAddr.sin_family << std::endl;
        std::cerr << "Server Port: " << ntohs(serverAddr.sin_port) << std::endl;
        std::cerr << "Server IP Address: " << inet_ntoa(serverAddr.sin_addr) << std::endl;

        inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

        if (connect(ConnectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Error connecting to server: " << WSAGetLastError() << std::endl;
            closesocket(ConnectSocket);
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

        std::string response;
        char buffer[1024];
        int bytesReceived;
        while ((bytesReceived = recv(ConnectSocket, buffer, sizeof(buffer), 0)) > 0) {
            response.append(buffer, bytesReceived);
        }

        if (bytesReceived < 0) {
            std::cerr << "Error receiving response: " << WSAGetLastError() << std::endl;
        }

        closesocket(ConnectSocket);
        WSACleanup();

    return response;
}

}

