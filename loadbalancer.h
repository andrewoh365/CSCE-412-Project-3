#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "request.h"
#include <queue>
#include <vector>
#include <climits>
#include "webserver.cpp"

class LoadBalancer {
public:
    LoadBalancer();  // Default constructor
    LoadBalancer(int serverCount);  // Constructor with server count

    int getCurrentTime();  // Get the current system time
    void advanceTime();  // Increment system time

    void queueRequest(const request& req);  // Add a request to the queue
    request fetchNextRequest();  // Fetch the next request from the queue

    bool isQueueEmpty();  // Check if the request queue is empty
    int getQueueLength();  // Get the current length of the queue

    void runCycle();  // Run a single cycle of load balancing
    void showStatistics();  // Show statistics of the load balancer

private:
    int timeElapsed;  // Tracks elapsed time
    int currentServerIndex;  // Index of the current server

    std::queue<request> jobQueue;  // Queue of requests
    std::vector<webserver> serverPool;  // Pool of web servers

    // Statistics
    int totalRequestsHandled;
    int failedRequests;
    int longestProcessingTime;
    int shortestProcessingTime;
    int serversAllocated;
    int serversFreed;
    int maxQueueCapacity;
    int minQueueCapacity;

    int activeServers;  // Current active servers
    int maxServers;  // Maximum number of servers allowed
};

#endif
