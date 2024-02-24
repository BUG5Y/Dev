#include <implant.h>
#include <xhttp.h>
#include <iostream>

// Use messagepack when sending data
// https://github.com/msgpack/msgpack-c/tree/cpp_master

using namespace implant;

std::string implant::beacon() {
    std::string url = "www.example.com/index.html";
    std::string response = http_get(url);
    std::cout << response << std::endl;
    return 0;
}