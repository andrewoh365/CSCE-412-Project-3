// webserver.h

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "request.h"  // Assuming request.h defines the request class

class WebServer {
  public:
    WebServer();
    WebServer(char c);
    void addRequest(request newReq, int currentTime);
    request getRequest();
    char getName();
    bool isReqDone(int currentTime);

  private:
    request req;
    int reqStartTime;
    char serverName;
};

#endif
