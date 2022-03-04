#include "clientsmanager.h"

extern string activeUsers[10000];
extern pthread_mutex_t clientsMutex;

extern int MAX_ACTIVE_USERS;
extern int MAX_ACTIVE_SESSIONS;

extern ActiveSession activeSessions[5000];
extern pthread_t sessionThreads[5000];

extern string filesOpenned[5000];



ClientsManager::ClientsManager(int PORT, int sockDescriptor, sockaddr_in clientAddr, int maxActiveUsers) : PORT(PORT), sockDescriptor(sockDescriptor), clientAddr(clientAddr), maxActiveUsers(maxActiveUsers)
{

}

ClientsManager::~ClientsManager()
{
    pthread_mutex_destroy(&clientsMutex);

}


void ClientsManager::startCM()
{

    for(int i = 0; i < maxActiveUsers; i++)
        {
            activeUsers[i] = "";
        }

    for(int i = 0; i < maxActiveSessions; i++)
        {
            filesOpenned[i] = "";
        }

    for(int i = 0; i < maxActiveSessions; i++)
        {
            activeSessions[i].setClear();
        }

    for(int i = 0; i < maxActiveSessions; i++)
        {
            sessionThreads[i] = pthread_t();
        }

    int error;
    if ((error = pthread_mutex_init(&clientsMutex, nullptr)) != 0)
        {
            perror("[SERVER] | [ERROR] : MUTEX INIT HAS FAILED :\n");
            perror(strerror(error));
        }

    map<int, pthread_t> threadIds;


    int idThread = 0;
    while (1)
        {
            cerr << "[SERVER] SERVER IS RUNNING (PORT:" << PORT << ")..." << endl;
            fflush(stdout);

            int clientDescriptor;
            int clientAddrLen = sizeof(clientAddr);

            clientDescriptor = accept(sockDescriptor, (struct sockaddr *)&clientAddr, (socklen_t *)&(clientAddrLen));

            if (clientDescriptor < 0)
                {
                    perror("[SERVER] | [ERROR] : accept()\n");
                    continue;
                }

            cerr << "[SERVER] | [CONNECTED] : CLIENT " << endl;
            fflush(stdout);

            threadIds[idThread] = pthread_t();

            int error;
            error = pthread_create(&(threadIds[idThread]), nullptr, &serveClient, &clientDescriptor);
            if (error != 0)
                {
                    perror("[SERVER] | [ERROR] : THREAD CAN'T BE CREATED :\n");
                    perror(strerror(error));
                }

            ++idThread;
        }

    pthread_mutex_destroy(&clientsMutex);

}


