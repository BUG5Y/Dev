#ifndef cmd
#define cmd

#include <string>
#include <Windows.h>

namespace wincmd {
bool execute_cmd(std::string& cmdString, std::string& current_dir, std::string& out_put, DWORD time_out); // const std::string& current_dir,
std::string current_dir = "."; // Initialize current_dir with a valid directory path
std::string out_put; // Initialize out_put with an empty string
DWORD time_out = 5000; // Initialize time_out with a timeout value in milliseconds

}

#endif