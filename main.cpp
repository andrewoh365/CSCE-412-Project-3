#include "loadbalancer.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

const int NUMWEBSERVERS = 5;

//create a request
request createRandomRequest()
{
    stringstream ips, ipd;
    request r;
    string job[2] = {"processing", "streaming"};
    ips << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256);
    ipd << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256) << "." << (rand() % 256);
    r.ipIn = ips.str();  // Set the source IP
    r.ipOut = ipd.str();  // Set the destination IP
    r.processTime = rand() % 500;  // Random processing time
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

    // User input
    cout << "Number of Servers: ";
    cin >> SERVERS;

    cout << "Total run time: ";
    cin >> TOTALTIME;

    // Seed random number generator with current system time
    srand(time(0));

    // Initialize output file and declare loadbalancer variable
    ofstream file("log.txt");
    LoadBalancer lb(SERVERS, &file);
 
    // Start off with a "full" queue
    int generated = 0;
    while (generated < SERVERS * 100) {
        request r = createRandomRequest();
        if (!isIPBlocked(r.ipIn)) {
            lb.queueRequest(createRandomRequest());
            generated++;
        }
    }

    file << "------- LOAD BALANCER LOG -------" << endl;
    // Loop through the total simulation time
    while (lb.getCurrentTime() < TOTALTIME) {
        lb.run();

        // Create new requests at random intervals
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

    // Output statistics at the end of the simulation
    lb.showStatistics();

    file.close();  // Close the log file
    cout << "----- Created file log.txt -----" << endl;

    return 0;  // Exit the program successfully
}