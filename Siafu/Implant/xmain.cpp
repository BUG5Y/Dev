#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include "implant.h"
#include <iostream>

int main() {
    // Call the beacon function
    std::string result = implant::beacon();

    // Output the result (if needed)
    std::cout << "Result: " << result << std::endl;

    return 0;
}
