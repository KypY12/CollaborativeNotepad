#ifndef ACTIVESESSION_H
#define ACTIVESESSION_H

#include <iostream>
#include <deque>
#include <pthread.h>
#include <string>

#include <QString>
#include <QStringBuilder>

#include "operation.h"


using namespace std;

class ActiveSession
{
    pthread_mutex_t lock;

    string user[2];
    string fileName;

    Operation fromUser[2];
    bool fromUserChanged[2];

    deque<Operation> toUser0;
    deque<Operation> toUser1;

    bool toUserChanged[2];

public:

    ActiveSession();
    ~ActiveSession();

    void setFileName(string fileName);
    bool isFull();

    int addUser(string username);
    void removeUser(int clientId);

    bool fromClientChanged(int idClient);
    bool toClientChanged(int idClient);

    void setOperationFromClient(int idClient, Operation &op);
    void setOperationToClient(int idClient, deque<Operation>& ops);

    Operation& getOperationFromClient(int idClient);
    deque<Operation>& getOperationToClient(int idClient);

    void lockFunction();
    void unlockFunction();

    bool isUser0();
    bool isUser1();

    void setClear();
    bool isClear();

    string getUser(int userId);

    friend class SessionManager;
};




#endif // ACTIVESESSION_H
