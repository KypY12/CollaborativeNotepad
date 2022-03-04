#ifndef CLIENTSMANAGER_H
#define CLIENTSMANAGER_H

#include <fstream>
#include <iostream>

#include <stdio.h>
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "commandmode.h"
#include "communication.h"
#include "editmode.h"
#include "loginoutmanager.h"

using namespace std;


static void* serveClient(void* arg)
{
    int clientDescriptor = *((int*)arg);
    pthread_detach(pthread_self());

    CommandMode cmdM(clientDescriptor);
    cmdM.activateCMMode();

}


class ClientsManager
{

private:

    int PORT;
    int sockDescriptor;
    struct sockaddr_in clientAddr;


public:

    int maxActiveUsers;
    int maxActiveSessions;


    ClientsManager() = delete;
    ClientsManager(int PORT, int sockDescriptor, struct sockaddr_in clientAddr, int maxActiveUsers);
    ~ClientsManager();

    void startCM();


};

#endif // CLIENTSMANAGER_H
