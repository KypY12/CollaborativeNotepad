#include "editmode.h"

pthread_mutex_t lockEdit;

bool isEditStillRunning = false;

EditMode::EditMode()
{

}

void EditMode::activateEMode()
{
    isEditStillRunning = true;

    int error;
    struct editModeThArgs thArgs;

    thArgs.clientId = clientId;
    thArgs.clientDescriptor = clientDescriptor;
    thArgs.currentSession = currentSession;

    if ((error = pthread_mutex_init(&lockEdit, nullptr)) != 0)
        {
            perror("[SERVER] | [ERROR] : MUTEX INIT HAS FAILED :\n");
            perror(strerror(error));
        }

    error = pthread_create(&(tid), nullptr, &sendUpdateToClient, &thArgs);
    if (error != 0)
        printf("\nThread can't be created : [%s]", strerror(error));

    pthread_mutex_lock(&lockEdit);
    while(isEditStillRunning)
        {
            pthread_mutex_unlock(&lockEdit);

            Operation * recvOp = commObj.recvOperationFromDescriptor();
            //usleep(1000);
            if(recvOp->getType() == EXIT)
                {
                    pthread_mutex_lock(&lockEdit);
                    isEditStillRunning = false;
                    commObj.sendStringToDescriptor("exit");

                    currentSession->lockFunction();
                    currentSession->removeUser(clientId);
                    currentSession->unlockFunction();
                    pthread_mutex_unlock(&lockEdit);

                    break;
                }
            else if(recvOp->getType() == DEFAULT)
                {
                    cerr << "UPDATE" << endl;

                }
            else
                {

                    pthread_mutex_lock(&lockEdit);
                    currentSession->lockFunction();
                    currentSession->setOperationFromClient(clientId, *recvOp);
                    while(currentSession->fromClientChanged(clientId))
                    {
                        currentSession->unlockFunction();
                        usleep(1000);
                        currentSession->lockFunction();
                    }
                    currentSession->unlockFunction();
                    pthread_mutex_unlock(&lockEdit);
                }

            pthread_mutex_lock(&lockEdit);
        }
    pthread_mutex_unlock(&lockEdit);


}

void EditMode::setClientDescriptor(int clientDescriptor)
{
    this->clientDescriptor = clientDescriptor;
    commObj.setDescriptor(clientDescriptor);
}

void EditMode::setClientId(int clientId)
{
    this->clientId = clientId;
}

void EditMode::setSession(ActiveSession *currentSessionParam)
{
    currentSession = currentSessionParam;
}


void *EditMode::sendUpdateToClient(void *arg)
{
    pthread_detach(pthread_self());
    deque<Operation> toClientOps;

    pthread_mutex_lock(&lockEdit);
    struct editModeThArgs * thData = (struct editModeThArgs *)arg;
    Communication comObj(thData->clientDescriptor);

    while(isEditStillRunning)
        {
            thData->currentSession->lockFunction();
            while(!(thData->currentSession->toClientChanged(thData->clientId)) && isEditStillRunning)
            {
                thData->currentSession->unlockFunction();
                pthread_mutex_unlock(&lockEdit);
                usleep(2000);
                pthread_mutex_lock(&lockEdit);
                thData->currentSession->lockFunction();
            }
            if(!(isEditStillRunning))
                {
                    thData->currentSession->unlockFunction();
                    pthread_mutex_unlock(&lockEdit);
                    break;
                }

            toClientOps = thData->currentSession->getOperationToClient(thData->clientId);

            thData->currentSession->unlockFunction();
            pthread_mutex_unlock(&lockEdit);


            if(toClientOps.size() == 1)
                {
                    comObj.sendStringToDescriptor("update");
                    comObj.sendStringToDescriptor("peace");
                    comObj.sendOperationToDescriptor(toClientOps[0]);
                }
            else
                {
                    comObj.sendStringToDescriptor("update");
                    comObj.sendStringToDescriptor("conflict");
                    comObj.sendOperationToDescriptor(toClientOps[0]);
                    comObj.sendOperationToDescriptor(toClientOps[1]);
                }

            toClientOps.clear();
            pthread_mutex_lock(&lockEdit);
        }

    pthread_mutex_unlock(&lockEdit);

    pthread_mutex_destroy(&lockEdit);

    pthread_exit(nullptr);
}

