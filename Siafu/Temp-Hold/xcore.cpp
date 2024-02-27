#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime> // for std::time
#include <sstream>
#include <cstdlib>
#include <zlib.h> // for CRC32
#include "xcore.h"
// #include "message.pb.h"

namespace xcore{

    std::vector<std::string> shuffleVector(const std::vector<std::string>& vec) {
        std::mt19937 rng(std::time(nullptr)); // Seed the random number generator
        std::vector<std::string> shuffledVec = vec; // Make a copy to shuffle
        std::shuffle(shuffledVec.begin(), shuffledVec.end(), rng);
        return shuffledVec;
    }

    // Example usage:
    void exampleUsage() {
        std::vector<std::string> shuffledDir1 = shuffleVector(directories1);
        for (const std::string& directory : shuffledDir1) {
            std::cout << directory << " ";
        }
        std::cout << std::endl;
    }

std::string subID[2];

std::string generateID() {
    int length = 40;
    std::string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string UID;
    for (int i = 0; i < length; ++i) {
        UID += characters[rand() % characters.size()];
    }
    
    // Assuming subOne() and subTwo() are meant to set values in subID
    subOne(); // Set subID[0]
    subTwo(); // Set subID[1]
    
    std::string tempID = UID + subID[2];

    // Calculate CRC32 hash
    uLong crc = crc32(0L, (const Bytef *)tempID.c_str(), tempID.length());

    // Convert CRC32 value to hexadecimal string
    std::stringstream ss;
    ss << std::hex << crc;
    std::string xID = ss.str();

    return xID;
}

void subOne() {
    subID[0] = "test"; // Replace with OS Version
}

void subTwo() {
    subID[1] = "Test"; // Replace with Registry key
}
/*
void setHello(Hello& message) {
    message.set_id(xcore::xID);
    message.set_name("Hello, MyMessage!");
}

void setcmdMessage(cmdMessage& message) {
    message.set_cmd("123");
    message.set_output("Hello, MyMessage!");
}

// Serialize a message to a string
template <typename MessageType>
std::string serializeMessage(const MessageType& message) {
    std::string serialized_message;
    message.SerializeToString(&serialized_message);
    return serialized_message;
}

// Deserialize a message from a string
template <typename MessageType>
MessageType deserializeMessage(const std::string& serialized_message) {
    MessageType message;
    message.ParseFromString(serialized_message);
    return message;
}

// Serialize to string
void cerealHello() {
    // Create a Hello message
    Hello message;
    
    // Set Hello message fields
    setHello(message);

    // Serialize Hello message to string
    std::string serialized_message = serializeMessage(message);
}
*/
}