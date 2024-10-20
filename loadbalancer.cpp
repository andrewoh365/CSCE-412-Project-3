#include "loadbalancer.h"
#include <iostream>

/**
 * @brief Default constructor that initializes system time, server count, and statistics.
 */
LoadBalancer::LoadBalancer() {
    time = 0;
    currentServerIndex = 0;
    activeServers = 0;
    maxServers = 0;

    // statistics
    totalRequestsHandled = 0;
    failedRequests = 0;
    longestProcessingTime = 0;
    shortestProcessingTime = INT_MAX;
    serversAllocated = 0;
    serversDeallocated = 0;
    maxQueueCapacity = INT_MAX;
    minQueueCapacity = 0;
}

/**
 * @brief Constructor that initializes the load balancer with a defined number of servers.
 * @param serverCount The number of servers to start with.
 */
LoadBalancer::LoadBalancer(int servers, std::ofstream* _log) : LoadBalancer() {
    log = _log;
    activeServers = maxServers = servers;
    minQueueCapacity = maxQueueCapacity = servers * 100;

    for (int i = 0; i < maxServers; i++) {
        webarray.push_back(webserver('A' + i));  // Initialize servers with unique names
    }
}

/**
 * @brief Retrieve the current time in the system.
 * @return The current system time.
 */
int LoadBalancer::getCurrentTime() {
    return time;
}

void LoadBalancer::queueBlockedRequest() {
    failedRequests++;
}

/**
 * @brief Increase the system time by one unit.
 */
void LoadBalancer::incTime() {
    time++;
}

/**
 * @brief Add a request to the queue, while updating processing statistics.
 * @param req The request to be added.
 */
void LoadBalancer::queueRequest(request req) {
    if (req.processTime > longestProcessingTime) {
        longestProcessingTime = req.processTime;
    }
    if (req.processTime < shortestProcessingTime) {
        shortestProcessingTime = req.processTime;
    }
    jobQueue.push(req);
    incTime();
}

/**
 * @brief Retrieve the next request from the queue.
 * @return The next request from the queue, or an empty request if the queue is empty.
 */
request LoadBalancer::fetchNextRequest() {
    incTime();
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
void LoadBalancer::run() {
    // Log the queue size every 50 time units
    if (getCurrentTime() % 50 == 0) {
        *log << "Time " << getCurrentTime() << ": " << getQueueLength() << " request(s) in queue." << std::endl;
    }

    // Check if the current server has finished processing its request
    if (!webarray[currentServerIndex].getAvailability() && webarray[currentServerIndex].isRequestDone(getCurrentTime())) {
        request req = webarray[currentServerIndex].getRequest();
        totalRequestsHandled++;
        *log << "Time " << getCurrentTime() << ": Server " << webarray[currentServerIndex].getName()
                  << " completed request from " << req.ipIn << " to " << req.ipOut << std::endl;
        webarray[currentServerIndex].setAvailability(true);
    }

    // Manage server allocation and deallocation based on queue size
    if (getQueueLength() > 10 * activeServers && maxServers >= activeServers + 1) {
        // Allocate additional server
        serversAllocated++;
        webarray.push_back(webserver('A' + webarray.size()));
        activeServers++;
        *log << "Time " << getCurrentTime() << ": Added new server " << webarray.back().getName()
                  << " (" << activeServers << " active servers)" << std::endl;
    } else if (getQueueLength() < 3 * activeServers && activeServers > 3 && webarray.back().getAvailability()) {
        // Free an idle server
        serversDeallocated++;
        *log << "Time " << getCurrentTime() << ": Freed server " << webarray.back().getName()
                  << " (" << --activeServers << " active servers)" << std::endl;
        webarray.pop_back();
        if (currentServerIndex >= activeServers) {
            currentServerIndex = 0;  // Reset index if necessary
        }
    }

    // Assign a new request to an available server
    if (webarray[currentServerIndex].getAvailability() && !isQueueEmpty()) {
        request req = fetchNextRequest();
        webarray[currentServerIndex].setRequest(req, getCurrentTime());
        webarray[currentServerIndex].setAvailability(false);
        *log << "Time " << getCurrentTime() << ": Server " << webarray[currentServerIndex].getName()
                  << " started processing request from " << req.ipIn << " to " << req.ipOut << std::endl;
    }

    incTime();
    currentServerIndex = (currentServerIndex + 1) % activeServers;  // Move to the next server
}

/**
 * @brief Display the final statistics after processing is complete.
 */
void LoadBalancer::showStatistics() {
    *log << "------- FINAL STATISTICS -------\n"
              << "Initial queue size: " << maxServers * 100 << "\n"
              << "Final queue size: " << getQueueLength() << "\n"
              << "Processing times: " << shortestProcessingTime << " - " << longestProcessingTime << "\n"
              << "Total requests handled: " << totalRequestsHandled << "\n"
              << "Failed requests: " << failedRequests << "\n"
              << "Total servers added: " << serversAllocated << "\n"
              << "Total servers freed: " << serversDeallocated << "\n";
}
