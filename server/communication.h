#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <iostream>
#include <string>
#include <deque>

#include "operation.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>


#define commBufferSize 256
#define smallCommBuffSize 64


extern int errno;

using namespace std;

class Communication
{
private:

    int commDescriptor;


public:

    Communication();
    Communication(int descriptor);

    void sendStringToDescriptor(string message);
    string recvStringFromDescriptor();

    void sendOperationToDescriptor(Operation &operation);
    Operation* recvOperationFromDescriptor();

    bool isMessageReceived();
    void sendByte(char byteValue);
    void setDescriptor(int descriptor);

};

#endif // COMMUNICATION_H
