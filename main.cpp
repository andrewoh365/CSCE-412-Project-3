#include "request.cpp"
#include "webserver.cpp"
#include "loadbalancer.h"

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <sstream>

using namespace std;

const int NUMWEBSERVERS = 5;

//create a request
request createRandomRequest()
{
    stringstream ips, ipd;
    request r;
    ips << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256);
    ipd << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256);
    r.ipIn = ips.str();  // Set the source IP
    r.ipOut = ipd.str();  // Set the destination IP
    r.processTime = rand() % 500;  // Random processing time
    return r;
}

int main()
{
    //random number generator
    srand(time(0));

    //create a loadbalancer
    loadbalancer lb;

    //start off with a "full" queue
    for (int i = 0; i < 10; i++)
    {
        request r = createRandomRequest();
        lb.addRequest(r);  // Add the request to the load balancer
    }

    //an array of webservers
    webserver webarray[NUMWEBSERVERS];
    for (int i = 0; i < NUMWEBSERVERS; i++)
    {
        webserver w((char)(i + 65));  // Create a new WebServer with a letter name
        webarray[i] = w;
        webarray[i].addRequest(lb.getRequest(), lb.getTime());  // Assign a request to each webserver
    }

    //loop
    while (lb.getTime() < 10000)  // Fix: add parentheses to getTime()
    {
        int currTime = lb.getTime();
        //check each webserver if it's done
        if (webarray[currTime % NUMWEBSERVERS].isRequestDone(currTime))
        {
            request r = webarray[currTime % NUMWEBSERVERS].getRequest();
            cout << "At " << currTime << " Webserver " << webarray[currTime % NUMWEBSERVERS].getName()
                 << " processed request from " << r.ipIn << " to " << r.ipOut << endl;
            //then give it a new request
            webarray[currTime % NUMWEBSERVERS].addRequest(lb.getRequest(), currTime);
        }

        //every random amount of time, we get a new request
        if (rand() % 10 == 0)
        {
            request r = createRandomRequest();  // Create a new random request
            lb.addRequest(r);  // Fix: pass the new request to addRequest()
        }

        lb.incTime();  // Increment the time in the load balancer
    }

    return 0;
}
