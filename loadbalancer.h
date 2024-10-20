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
    LoadBalancer(int servers, ofstream* _log);  // Constructor with server count

    void setCurrentTime();
    int getCurrentTime();  // Get the current system time
    void incTime();  // Increment system time

    void queueRequest(request req);  // Add a request to the queue
    request fetchNextRequest();  // Fetch the next request from the queue
    void queueBlockedRequest();
    bool isQueueEmpty();  // Check if the request queue is empty
    int getQueueLength();  // Get the current length of the queue

    void run();  // Run a single cycle of load balancing
    void showStatistics();  // Show statistics of the load balancer

private:
    int time;  // Tracks elapsed time
    int currentServerIndex;  // Index of the current server
    ofstream* log;
    std::queue<request> jobQueue;  // Queue of requests
    std::vector<webserver> webarray;  // Pool of web servers

    // Statistics
    int totalRequestsHandled;
    int failedRequests;
    int longestProcessingTime;
    int shortestProcessingTime;
    int serversAllocated;
    int serversDeallocated;
    int maxQueueCapacity;
    int minQueueCapacity;

    int activeServers;  // Current active servers
    int maxServers;  // Maximum number of servers allowed
};

#endif
