#include <string>
#include <cstdlib>
#include <ctime>
#include "request.h"

// Helper function to generate a random IP address
std::string generateRandomIP() {
    return std::to_string(rand() % 256) + "." + 
           std::to_string(rand() % 256) + "." + 
           std::to_string(rand() % 256) + "." + 
           std::to_string(rand() % 256);
}

// Helper function to generate random process time (between 1 and 100 clock cycles)
int generateRandomProcessTime() {
    return rand() % 100 + 1;  // Random time between 1 and 100
}

// Helper function to generate a random job type ('P' for processing, 'S' for streaming)
char generateRandomJobType() {
    return (rand() % 2 == 0) ? 'P' : 'S';
}

// Function to generate a random request
request generateRandomRequest() {
    request r;
    r.ipIn = generateRandomIP();
    r.ipOut = generateRandomIP();
    r.processTime = generateRandomProcessTime();
    r.jobType = generateRandomJobType();
    return r;
}
