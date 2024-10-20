// webserver.cpp

#include "webserver.h"  // Assuming this is the correct header file name for your webserver class

void WebServer::addRequest(request newReq, int currentTime) {
    req = newReq; 
    reqStartTime = currentTime;
}

request WebServer::getRequest() {
    return req;
}

char WebServer::getName() {
    return serverName;
}

bool WebServer::isReqDone(int currentTime) {
    return currentTime > (reqStartTime + req.processTime);
}
