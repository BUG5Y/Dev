#ifndef PBPARSE_H
#define PBPARSE_H

#include <pbparse.cpp>

namespace pbParse {
// Function to set Hello message
void setHello(Hello& message);

// Function to set cmdMessage
void setcmdMessage(cmdMessage& message);

// Serialize a message to a string
template <typename MessageType>
std::string serializeMessage(const MessageType& message);

// Deserialize a message from a string
template <typename MessageType>
MessageType deserializeMessage(const std::string& serialized_message);

}
#endif 



