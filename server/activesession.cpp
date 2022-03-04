#include "activesession.h"



ActiveSession::ActiveSession()
{
    user[0] = "";
    user[1] = "";

    fileName = "";

    string tempString = "";

    fromUserChanged[0] = false;
    fromUserChanged[1] = false;

    toUserChanged[0] = false;
    toUserChanged[1] = false;

    if (pthread_mutex_init(&lock, nullptr) != 0)
        {
            printf("\n mutex init has failed\n");
        }

}

ActiveSession::~ActiveSession()
{
    pthread_mutex_destroy(&lock);
}

void ActiveSession::setFileName(string fileName)
{
    this->fileName = fileName;
}

bool ActiveSession::isFull()
{
    if(user[0].empty() || user[1].empty())
        return false;
    else
        return true;

}

int ActiveSession::addUser(string username)
{

    if(user[0].empty())
        {
            user[0] = username;
            return 0;
        }
    else if(user[1].empty())
        {
            user[1] = username;
            return 1;
        }
    else
        {
            return -1;
        }

}

void ActiveSession::removeUser(int clientId)
{
    if(clientId == 0 || clientId == 1)
        user[clientId].clear();
}

bool ActiveSession::fromClientChanged(int idClient)
{
    if(idClient == 0 || idClient == 1)
        return fromUserChanged[idClient];
    return false;
}


bool ActiveSession::toClientChanged(int idClient)
{
    if(idClient == 0 || idClient == 1)
        return toUserChanged[idClient];
    return false;
}


void ActiveSession::setOperationFromClient(int idClient, Operation &op)
{
    fromUserChanged[idClient] = true;
    fromUser[idClient] = op;
}


void ActiveSession::setOperationToClient(int idClient, deque<Operation>& ops)
{
    toUserChanged[idClient] = true;
    if(idClient == 0)
        {
            toUser0 = ops;
        }
    else
        {
            toUser1 = ops;
        }
}


Operation& ActiveSession::getOperationFromClient(int idClient)
{
    fromUserChanged[idClient] = false;
    return fromUser[idClient];
}


deque<Operation>& ActiveSession::getOperationToClient(int idClient)
{
    toUserChanged[idClient] = false;
    if(idClient == 0)
        {
            return toUser0;
        }
    else
        {
            return toUser1;
        }
}

void ActiveSession::lockFunction()
{
    pthread_mutex_lock(&lock);
}

void ActiveSession::unlockFunction()
{
    pthread_mutex_unlock(&lock);
}

bool ActiveSession::isUser0()
{
    if(user[0].empty())
        return false;
    else
        return true;
}

bool ActiveSession::isUser1()
{
    if(user[1].empty())
        return false;
    else
        return true;
}

void ActiveSession::setClear()
{
    user[0].clear();
    user[1].clear();
    fileName.clear();
}

bool ActiveSession::isClear()
{
    if(user[0] == "" && user[1] == "" && fileName == "")
        return true;
    else
        return false;
}

string ActiveSession::getUser(int userId)
{
    if(userId == 0 || userId == 1)
        return user[userId];

    return "";
}







