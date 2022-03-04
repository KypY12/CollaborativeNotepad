#include "sessionmanager.h"

extern string activeUsers[10000];
extern pthread_mutex_t clientsMutex;

extern int MAX_ACTIVE_USERS;
extern int MAX_ACTIVE_SESSIONS;

extern ActiveSession activeSessions[5000];
extern pthread_t sessionThreads[5000];
extern pthread_mutex_t sessionMutex[5000];

extern string filesOpenned[5000];



SessionManager::SessionManager(int sessionId) : sessionId(sessionId)
{
    manageSession();
}


void SessionManager::applyOperation(Operation &op)
{
    TypeOfOp typeOp = op.getType();
    QString textToUse;


    switch(typeOp)
        {
        case INSERT:

            textToUse = op.getCharsInserted().c_str();
            fileBuffer.insert(op.getPosition(), textToUse);

            break;

        case REMOVE:

            fileBuffer.remove(op.getPosition(), op.getNoChars());

            break;

        case MIXED:

            fileBuffer.remove(op.getPosition(), op.getNoChars());
            textToUse = op.getCharsInserted().c_str();
            fileBuffer.insert(op.getPosition(), textToUse);

            break;

        default:
            return;
        }

}


void SessionManager::manageSession()
{
    fstream fileToWrite;


    bool updateFile = false;
    activeSessions[sessionId].lockFunction();
    while(activeSessions[sessionId].isUser0() || activeSessions[sessionId].isUser1()) // cat timp exista un user in sesiunea curenta
        {
            if(activeSessions[sessionId].isUser0() && activeSessions[sessionId].isUser1())
            {
                bool changedUsr0 = activeSessions[sessionId].fromClientChanged(0);
                bool changedUsr1 = activeSessions[sessionId].fromClientChanged(1);

                if(changedUsr0 && !changedUsr1)
                    {
                        Operation op = activeSessions[sessionId].getOperationFromClient(0);

                        applyOperation(op);

                        deque<Operation> sendOp;
                        sendOp.push_back(op);

                        activeSessions[sessionId].setOperationToClient(1,sendOp);
                        updateFile = true;

                    }
                else if(!changedUsr0 && changedUsr1)
                    {
                        Operation op = activeSessions[sessionId].getOperationFromClient(1);

                        applyOperation(op);

                        deque<Operation> sendOp;
                        sendOp.push_back(op);

                        activeSessions[sessionId].setOperationToClient(0,sendOp);
                        updateFile = true;

                    }
                else if(changedUsr0 && changedUsr1)
                    {
                        Operation op0 = activeSessions[sessionId].getOperationFromClient(0);
                        Operation op1 = activeSessions[sessionId].getOperationFromClient(1);

                        applyOperation(op0);
                        applyOperation(op1);

                        deque<Operation> sendOp;


                        activeSessions[sessionId].setClear();

                        sendOp.push_back(op1);
                        activeSessions[sessionId].setOperationToClient(0, sendOp);

                        sendOp.push_front(op0);
                        activeSessions[sessionId].setOperationToClient(1,sendOp);


                        updateFile = true;

                    }
            }
            else if(activeSessions[sessionId].isUser0() && !(activeSessions[sessionId].isUser1()))
            {
                bool changedUsr0 = activeSessions[sessionId].fromClientChanged(0);

                if(changedUsr0)
                    {
                        Operation op = activeSessions[sessionId].getOperationFromClient(0);

                        applyOperation(op);

                        updateFile = true;

                    }
            }
            else if(activeSessions[sessionId].isUser1() && !(activeSessions[sessionId].isUser0()))
            {
                bool changedUsr1 = activeSessions[sessionId].fromClientChanged(1);

                if(changedUsr1)
                    {
                        Operation op = activeSessions[sessionId].getOperationFromClient(1);

                        applyOperation(op);

                        updateFile = true;

                    }
            }


            if(updateFile)
                {
                    fileToWrite.open("../files/"+activeSessions[sessionId].fileName, ios::out | ios::trunc);
                    fileToWrite << fileBuffer.toStdString();
                    fileToWrite.close();

                    updateFile = false;
                }

            activeSessions[sessionId].unlockFunction();
            usleep(1000);
            activeSessions[sessionId].lockFunction();
        }


    activeSessions[sessionId].unlockFunction();
    activeSessions[sessionId].setClear();
    filesOpenned[sessionId].clear();

}
