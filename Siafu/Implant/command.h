#ifndef COMMAND_H
#define COMMAND_H
#include <string>
#include "xhttp.h"

namespace command {
// Function prototype for createProc
std::string output;
int createProc(char* a);
void parseCmd(const std::string& cmdString);
extern char cmdProc[];
extern char psProc[];
}
#endif