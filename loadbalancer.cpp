#include "loadbalancer.h"
#include <iostream>

// Initialize system time, server count, and statistics.
LoadBalancer::LoadBalancer() {
    time = 0;
    currentServerIndex = 0;
    activeServers = 0;
    maxServers = 0;

    // Stats
    totalRequestsHandled = 0;
    failedRequests = 0;
    longestProcessingTime = 0;
    shortestProcessingTime = INT_MAX;
    serversAllocated = 0;
    serversDeallocated = 0;
    maxQueueCapacity = INT_MAX;
    minQueueCapacity = 0;
}

LoadBalancer::LoadBalancer(int servers, std::ofstream* _log) : LoadBalancer() {
    log = _log;
    activeServers = maxServers = servers;
    minQueueCapacity = maxQueueCapacity = servers * 100;

    for (int i = 0; i < maxServers; i++) {
        webarray.push_back(webserver('A' + i));  // Initialize servers with unique names
    }
}

int LoadBalancer::getCurrentTime() {
    return time;
}

void LoadBalancer::queueBlockedRequest() {
    failedRequests++;
}

void LoadBalancer::incTime() {
    time++;
}

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

request LoadBalancer::fetchNextRequest() {
    incTime();
    if (!jobQueue.empty()) {
        request req = jobQueue.front();
        jobQueue.pop();
        return req;
    }
    return request();  // Return an empty request if the queue is empty
}

bool LoadBalancer::isQueueEmpty() {
    return jobQueue.empty();
}

int LoadBalancer::getQueueLength() {
    return jobQueue.size();
}

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
    currentServerIndex = (currentServerIndex + 1) % activeServers;
}

// Display the final statistics after processing is complete.

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
