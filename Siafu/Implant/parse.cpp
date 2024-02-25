#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void parseHttpResponse(const std::string& response, std::map<std::string, std::string>& parameters) {
    std::istringstream responseStream(response);
    std::string line;

    // Read and parse the status line
    std::getline(responseStream, line);
    std::vector<std::string> statusLineParts = split(line, ' ');
    if (statusLineParts.size() < 3) {
        std::cerr << "Invalid HTTP response status line\n";
        return;
    }
    std::string httpVersion = statusLineParts[0];
    int statusCode = std::stoi(statusLineParts[1]);
    std::string reasonPhrase = statusLineParts[2];

    // Read and parse headers
    std::string header;
    while (std::getline(responseStream, line) && line != "\r") {
        header += line + "\n";
        // Check if the line contains parameters
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 2); // Skip ': ' after key
            parameters[key] = value;
        }
    }

    // Read and output the response body
    std::string body;
    while (std::getline(responseStream, line)) {
        body += line + "\n";
    }

    // Output parsed data
    std::cout << "HTTP Version: " << httpVersion << std::endl;
    std::cout << "Status Code: " << statusCode << std::endl;
    std::cout << "Reason Phrase: " << reasonPhrase << std::endl;
    std::cout << "Headers:\n" << header << std::endl;
    std::cout << "Body:\n" << body << std::endl;
    
    // Print the value of "hello" parameter if it exists
    if (parameters.count("hello") > 0) {
        std::cout << "Value of 'hello' parameter: " << parameters["hello"] << std::endl;
    }
}

