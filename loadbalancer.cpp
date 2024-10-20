// loadbalancer.cpp

#include "loadbalancer.h"  // Make sure this header file exists and is correctly included

int LoadBalancer::getTime() {
    return systemTime;
}

void LoadBalancer::incTime() {
    systemTime++;
}

void LoadBalancer::addRequest(Request r) {
    requestQueue.enqueue(r);
    incTime();
}

Request LoadBalancer::getRequest() {
    incTime();
    if (!requestQueue.isEmpty()) {  // Check if the queue is not empty
        Request r = requestQueue.dequeue();  // Dequeue the request
        return r;
    }
    return Request();  // Return a default request object if the queue is empty
}

bool LoadBalancer::isRequestQueueEmpty() {
    return requestQueue.isEmpty();
}
