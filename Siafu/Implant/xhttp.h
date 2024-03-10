#ifndef xHTTP_H
#define xHTTP_H

#include <string>
#include <vector>
#include <cstdint>
#include <Winsock2.h>
#include <map>

namespace xhttp {
    extern std::string host;
    extern std::string url;
    extern std::string port;
    extern std::vector<uint32_t> req_body;
    extern SOCKET ConnectSocket;
    extern std::string cmdstr;
    extern std::string cmd;
    extern int key;
    extern std::string cmdResponse;
    extern std::string cmdGroup;
    extern std::string cmdString;

    std::string http_get(const std::string& url);
    std::vector<char> receive_data(SOCKET ConnectSocket);
    SOCKET create_socket(const std::string& host, int port);
    std::string extractCMD(const std::vector<char>& data);
    std::string base64_encode(const std::string &in);
    
struct Command {
    std::string Group;
    std::string String;
    std::string Response;

};

    typedef std::map<int, Command> CommandQueue;

void addToQueue(CommandQueue& queue, int key, const std::string& cmdValue, const std::string& cmdString, const std::string& cmdResponse);
void removeFromQueue(CommandQueue& queue, int key);
// Define a type alias for the map that will store the queue
  // You can use unordered_map if order doesn't matter

}

#endif
