#include <QApplication>
#include "mainwindow.h"
#include "clientsmanager.h"

#include <deque>
#include <fstream>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define PORT 2024
#define LISTEN_QUEUE_SIZE 100


using namespace std;


string activeUsers[10000];
pthread_mutex_t clientsMutex;

int MAX_ACTIVE_USERS;
int MAX_ACTIVE_SESSIONS;

ActiveSession activeSessions[5000];
pthread_t sessionThreads[5000];

pthread_mutex_t sessionMutex[5000];

string filesOpenned[5000];


int main()
{
    MAX_ACTIVE_USERS = 10000;
    MAX_ACTIVE_SESSIONS = 5000;


    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    int sockDescriptor;
    if ((sockDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("[SERVER] | [ERROR] : socket()\n");
            return errno;
        }

    int optval = 1;
    setsockopt(sockDescriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    bzero(&serverAddr, sizeof(serverAddr));
    bzero(&clientAddr, sizeof(clientAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockDescriptor, (struct sockaddr *)&serverAddr, (socklen_t)sizeof(struct sockaddr)) == -1)
        {
            perror("[SERVER] | [ERROR] : bind()\n");
            return errno;
        }

    if (listen(sockDescriptor, LISTEN_QUEUE_SIZE) == -1)
        {
            perror("[SERVER] | [ERROR] : listen()\n");
            return errno;
        }


    ClientsManager clientM(PORT, sockDescriptor, clientAddr, MAX_ACTIVE_USERS);
    clientM.startCM();


    return 0;
}
