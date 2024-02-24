#ifndef xHTTP_H
#define xHTTP_H

#include <string>

namespace xhttp {
    bool parse_url(const std::string& url, std::string& protocol, std::string& host, int& port, std::string& path);
    std::string http_get(const std::string& url);
}

#endif
