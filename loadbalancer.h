#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "request.h"
#include <queue>
#include <vector>
#include <climits>
#include <fstream>
#include "webserver.cpp"

class LoadBalancer {
public:
    LoadBalancer();
    LoadBalancer(int servers, ofstream* _log);

    void setCurrentTime();
    // Get the current system time
    int getCurrentTime();
    // Increment system time
    void incTime();
     // Add a request to the queue
    void queueRequest(request req);
    // Fetch the next request from the queue
    request fetchNextRequest();
    void queueBlockedRequest();
    // Check if the request queue is empty
    bool isQueueEmpty();
    // Get the current length of the queue
    int getQueueLength();
    // Run a single cycle of load balancing
    void run();
    // Show statistics of the load balancer
    void showStatistics();

private:
    // Track elapsed time
    int time;
    // Index of the current server
    int currentServerIndex;
    ofstream* log;
    std::queue<request> jobQueue;
    std::vector<webserver> webarray;

    // Statistics
    int totalRequestsHandled;
    int failedRequests;
    int longestProcessingTime;
    int shortestProcessingTime;
    int serversAllocated;
    int serversDeallocated;
    int maxQueueCapacity;
    int minQueueCapacity;
    // Current active servers
    int activeServers;
    // Maximum number of servers allowed
    int maxServers;
};

#endif
