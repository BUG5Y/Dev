#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include "message.pb.h"

namespace pbParse {

void setHello(Hello& message) {
    message.set_id(123);
    message.set_name("Hello, MyMessage!");
}

void setcmdMessage(cmdMessage& message) {
    message.set_cmd(123);
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
    Hello(message);
    setHello(message);
    std::string serialized_message = serializeMessage(message);
}
}
/*
void eatCerealCMDResponse() {
    cmdMessage message;
    cmdMessage(message);
    // Deserialize MyMessage from string
    cmdMessage deserialized_message = deserializeMessage<cmdMessage>(serialized_message);
}
*/

/*
    // Serialize MyMessage to string
    std::string serialized_message = serializeMessage(message);

    // Deserialize MyMessage from string
    MyMessage deserialized_message = deserializeMessage<MyMessage>(serialized_message);

*/

