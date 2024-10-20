//webserver.cpp
#ifndef WEBSERVER_CPP
#define WEBSERVER_CPP

#include "request.h"

class webserver
{
  public:
    webserver()
    {
      requestStartTime = 0;
      servername = ' ';
      available = true;
    }
    webserver (char c) : webserver()
    {
      servername = c;
    }
    void addRequest(request req, int currTime)
    {
      r = req;
      requestStartTime = currTime;
    }
    request getRequest()
    {
      return r;
    }
    void setRequest(request req, int currTime)
    {
      r = req;
      requestStartTime = currTime;
    }
    char getName()
    {
      return servername;
    }
    bool isRequestDone(int currTime)
    {
      return (currTime >= (requestStartTime + r.processTime));
    }
    bool getAvailability() 
    {
      return available;
    }
    void setAvailability(bool temp) 
    {
      available = temp;
    }

  private:
    request r;
    int requestStartTime;
    char servername;
    bool available;
};

#endif