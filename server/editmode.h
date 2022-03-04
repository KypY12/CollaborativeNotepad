#ifndef EDITMODE_H
#define EDITMODE_H

#include <iostream>
#include <string>
#include <pthread.h>

#include "activesession.h"
#include "communication.h"

using namespace std;


struct editModeThArgs
{
    int clientId;
    int clientDescriptor;
    ActiveSession * currentSession;
};



class EditMode
{

private:
    int clientDescriptor;
    int clientId;

    ActiveSession * currentSession;


    Communication commObj;

public:

    EditMode();

    void activateEMode();

    void setClientDescriptor(int clientDescriptor);
    void setClientId(int clientId);
    void setSession(ActiveSession * currentSessionParam);

    static void * sendUpdateToClient(void * arg);
    pthread_t tid;


};

#endif // EDITMODE_H
