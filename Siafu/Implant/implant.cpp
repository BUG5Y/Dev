#include <implant.h>
#include <xhttp.h>

#include <iostream>

// Use messagepack when sending data
// https://github.com/msgpack/msgpack-c/tree/cpp_master

using namespace implant;

std::string implant::beacon(const std::string& url) {
    std::string url = "www.example.com/index.html";
    std::string response = xhttp::http_get(url);
    std::cout << response << std::endl;
    return response;
}