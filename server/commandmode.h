#ifndef COMMANDMODE_H
#define COMMANDMODE_H

#include <iostream>
#include <string>
#include <deque>
#include <map>

#include "activesession.h"
#include "communication.h"
#include "editmode.h"
#include "filemanager.h"
#include "loginoutmanager.h"
#include "sessionmanager.h"




static void *createSessionThread(void * sessionId)
{

    int currentSession = *((int*)sessionId);

    pthread_detach(pthread_self());
    SessionManager sm(currentSession);

    pthread_exit(nullptr);
}



using namespace std;


enum AvailableCommands
{
    errorF = 0,
    quitF = 1,
    registerF = 2,
    loginF = 3,
    logoutF = 4,
    createF = 5,
    openF = 6,
    downloadF = 7,
    deleteF = 8,
    permissionF = 9
};




class CommandMode
{

private:

    int clientDescriptor;

    int activeUserID = -1;

    Communication commObj;

    bool loggedIn = false;
    string username;
    string fileName;

    FileManager fm;
    LogInOutManager log = {"usr", "pass", clientDescriptor};

    void processCommand(string messageFromClient);
    AvailableCommands convertToEnum(string message);

public:

    CommandMode() = delete;
    CommandMode(int clientDescriptor);

    void activateCMMode();

};

#endif // COMMANDMODE_H
