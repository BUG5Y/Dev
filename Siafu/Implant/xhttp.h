#ifndef xHTTP_H
#define xHTTP_H

#include <WinSock2.h>
#include <string>
#include <map>

namespace xhttp {
    bool parse_url(const std::string& url, std::string& protocol, std::string& host, int& port, std::string& path);
    std::string http_get(const std::string& url); // , const std::map<std::string, std::string>& queryParameters
    
    struct RequestParam {
    std::map<std::string, std::string> queryParameters;
    };
}

#endif
