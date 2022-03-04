#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include <QString>
#include <QStringBuilder>

#include "activesession.h"
#include "operation.h"

extern pthread_mutex_t sessionMutex[5000];

using namespace std;

class SessionManager
{

private:

    int sessionId;

    QString fileBuffer;

    void applyOperation(Operation &op);

    void manageSession();

public:

    SessionManager() = delete;
    SessionManager(int sessionId);



};

#endif // SESSIONMANAGER_H
