#include "loadbalancer.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

const int NUMWEBSERVERS = 5;

// Create a request
request createRandomRequest()
{
    stringstream ips, ipd;
    request r;
    string job[2] = {"processing", "streaming"};
    ips << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256);
    ipd << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256);
    // Set the source IP
    r.ipIn = ips.str();
    // Set the destination IP
    r.ipOut = ipd.str();
    // Random processing time
    r.processTime = rand() % 500;
    r.jobType = job[rand() % 2];
    return r;
}

unsigned long toLong(string ip)
{
    istringstream IPstring(ip);
    unsigned long a, b, c, d;
    char temp;
    IPstring >> a>> temp >> b >> temp >> c >> temp >> d;
    return (a << 24) + (b << 16) + (c << 8) + d;
}

bool isIPBlocked(string ip)
{
    vector<pair<unsigned long, unsigned long>> blockedIPRanges =
    {
        {toLong("70.0.0.0"), toLong("79.255.255.255")},
        {toLong("110.0.0.0"), toLong("119.255.255.255")},
        {toLong("160.0.0.0"), toLong("169.255.255.255")}
    };
    unsigned long ipAsLong = toLong(ip);
    for (const auto& range : blockedIPRanges)
    {
        if (ipAsLong >= range.first && ipAsLong <= range.second)
        {
            return true;
        }
    }
    return false;
}

int main() {
    int SERVERS;        
    int TOTALTIME;     

    cout << "Number of Servers: ";
    cin >> SERVERS;

    cout << "Total run time: ";
    cin >> TOTALTIME;

    srand(time(0));

    ofstream file("log.txt");
    LoadBalancer lb(SERVERS, &file);

    // Creates full server queue
    int generated = 0;
    while (generated < SERVERS * 100) {
        request r = createRandomRequest();
        if (!isIPBlocked(r.ipIn)) {
            lb.queueRequest(createRandomRequest());
            generated++;
        }
    }

    // Loops through and creates requests
    file << "------- LOAD BALANCER LOG -------" << endl;
    while (lb.getCurrentTime() < TOTALTIME) {
        lb.run();

        if (rand() % 4 == 0) {
            request r = createRandomRequest();
            if (isIPBlocked(r.ipIn)) {
                file << "Time " << lb.getCurrentTime() << ": Request from " << r.ipIn << " rejected" << endl;
                lb.queueBlockedRequest();
            } else {
                lb.queueRequest(r);
                file << "Time " << lb.getCurrentTime() << ": New " << r.jobType << " request added to queue" << endl;
            }
        }
    }

    // Show statistics after simulation ends
    lb.showStatistics();

    file.close();
    cout << "----- Created file log.txt -----" << endl;

    return 0;
}