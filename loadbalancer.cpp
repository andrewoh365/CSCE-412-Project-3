#include "loadbalancer.h"
#include <iostream>

/**
 * @brief Default constructor that initializes system time, server count, and statistics.
 */
LoadBalancer::LoadBalancer() {
    timeElapsed = 0;
    currentServerIndex = 0;
    activeServers = 0;
    maxServers = 0;

    // statistics
    totalRequestsHandled = 0;
    failedRequests = 0;
    longestProcessingTime = 0;
    shortestProcessingTime = INT_MAX;
    serversAllocated = 0;
    serversFreed = 0;
    maxQueueCapacity = INT_MAX;
    minQueueCapacity = 0;
}

/**
 * @brief Constructor that initializes the load balancer with a defined number of servers.
 * @param serverCount The number of servers to start with.
 */
LoadBalancer::LoadBalancer(int serverCount) : LoadBalancer() {
    activeServers = maxServers = serverCount;
    minQueueCapacity = maxQueueCapacity = serverCount * 100;

    for (int i = 0; i < maxServers; i++) {
        serverPool.push_back(webserver('A' + i));  // Initialize servers with unique names
    }
}

/**
 * @brief Retrieve the current time in the system.
 * @return The current system time.
 */
int LoadBalancer::getCurrentTime() {
    return timeElapsed;
}

/**
 * @brief Increase the system time by one unit.
 */
void LoadBalancer::advanceTime() {
    timeElapsed++;
}

/**
 * @brief Add a request to the queue, while updating processing statistics.
 * @param req The request to be added.
 */
void LoadBalancer::queueRequest(const request& req) {
    if (req.processTime > longestProcessingTime) {
        longestProcessingTime = req.processTime;
    }
    if (req.processTime < shortestProcessingTime) {
        shortestProcessingTime = req.processTime;
    }
    jobQueue.push(req);
    advanceTime();
}

/**
 * @brief Retrieve the next request from the queue.
 * @return The next request from the queue, or an empty request if the queue is empty.
 */
request LoadBalancer::fetchNextRequest() {
    advanceTime();
    if (!jobQueue.empty()) {
        request req = jobQueue.front();
        jobQueue.pop();
        return req;
    }
    return request();  // Return an empty request if the queue is empty
}

/**
 * @brief Check if the queue has no requests.
 * @return True if the queue is empty, false otherwise.
 */
bool LoadBalancer::isQueueEmpty() {
    return jobQueue.empty();
}

/**
 * @brief Get the number of requests currently in the queue.
 * @return The number of requests in the queue.
 */
int LoadBalancer::getQueueLength() {
    return jobQueue.size();
}

/**
 * @brief Perform one cycle of the load balancing process.
 * This manages request assignment, server availability, and statistics.
 */
void LoadBalancer::runCycle() {
    // Log the queue size every 50 time units
    if (getCurrentTime() % 50 == 0) {
        std::cout << "Time " << getCurrentTime() << ": " << getQueueLength() << " request(s) in queue." << std::endl;
    }

    // Check if the current server has finished processing its request
    if (!serverPool[currentServerIndex].getAvailability() && serverPool[currentServerIndex].isRequestDone(getCurrentTime())) {
        request req = serverPool[currentServerIndex].getRequest();
        totalRequestsHandled++;
        std::cout << "Time " << getCurrentTime() << ": Server " << serverPool[currentServerIndex].getName()
                  << " completed request from " << req.ipIn << " to " << req.ipOut << std::endl;
        serverPool[currentServerIndex].setAvailability(true);
    }

    // Manage server allocation and deallocation based on queue size
    if (getQueueLength() > 10 * activeServers && maxServers >= activeServers + 1) {
        // Allocate additional server
        serversAllocated++;
        serverPool.push_back(webserver('A' + serverPool.size()));
        activeServers++;
        std::cout << "Time " << getCurrentTime() << ": Added new server " << serverPool.back().getName()
                  << " (" << activeServers << " active servers)" << std::endl;
    } else if (getQueueLength() < 3 * activeServers && activeServers > 3 && serverPool.back().getAvailability()) {
        // Free an idle server
        serversFreed++;
        std::cout << "Time " << getCurrentTime() << ": Freed server " << serverPool.back().getName()
                  << " (" << --activeServers << " active servers)" << std::endl;
        serverPool.pop_back();
        if (currentServerIndex >= activeServers) {
            currentServerIndex = 0;  // Reset index if necessary
        }
    }

    // Assign a new request to an available server
    if (serverPool[currentServerIndex].getAvailability() && !isQueueEmpty()) {
        request req = fetchNextRequest();
        serverPool[currentServerIndex].setRequest(req, getCurrentTime());
        serverPool[currentServerIndex].setAvailability(false);
        std::cout << "Time " << getCurrentTime() << ": Server " << serverPool[currentServerIndex].getName()
                  << " started processing request from " << req.ipIn << " to " << req.ipOut << std::endl;
    }

    advanceTime();
    currentServerIndex = (currentServerIndex + 1) % activeServers;  // Move to the next server
}

/**
 * @brief Display the final statistics after processing is complete.
 */
void LoadBalancer::showStatistics() {
    std::cout << "------- FINAL STATISTICS -------\n"
              << "Initial queue size: " << maxServers * 100 << "\n"
              << "Final queue size: " << getQueueLength() << "\n"
              << "Processing times: " << shortestProcessingTime << " - " << longestProcessingTime << "\n"
              << "Total requests handled: " << totalRequestsHandled << "\n"
              << "Failed requests: " << failedRequests << "\n"
              << "Total servers added: " << serversAllocated << "\n"
              << "Total servers freed: " << serversFreed << "\n";
}
