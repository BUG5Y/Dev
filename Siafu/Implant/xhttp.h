#ifndef xHTTP_H
#define xHTTP_H

#include <string>
#include <vector>
#include <cstdint>
#include <Winsock2.h>

namespace xhttp {
    extern std::string host;
    extern std::string url;
    extern std::string port;
    extern std::vector<uint32_t> req_body;
    extern SOCKET ConnectSocket;

    std::string http_get(const std::string& url);
    std::vector<uint32_t> receive_data(SOCKET ConnectSocket);
    SOCKET create_socket(const std::string& host, int port);
}

#endif
