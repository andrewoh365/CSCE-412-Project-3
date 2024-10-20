#ifndef REQUEST_H
#define REQUEST_H
#include <iostream>
#include <string>
using namespace std;

struct request {
    std::string ipIn;
    std::string ipOut;
    int processTime;
    char jobType;  // 'P' for processing, 'S' for streaming
};

#endif
