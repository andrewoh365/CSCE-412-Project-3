#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "request.h"  // Include the header file for the request struct
#include <queue>

class loadbalancer
{
  public:
    loadbalancer()
    {
      systemTime = 0;
    }
    int getTime();
    void incTime();
    void addRequest (request r);
    request getRequest();
    bool isRequestQueueEmpty();
  private:
    int systemTime;
    std::queue<request> requestQueue;  // Use std::queue
};

#endif
