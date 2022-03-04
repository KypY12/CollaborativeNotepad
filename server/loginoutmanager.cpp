#include "loginoutmanager.h"

extern string activeUsers[10000];
extern pthread_mutex_t clientsMutex;

extern int MAX_ACTIVE_USERS;
extern int MAX_ACTIVE_SESSIONS;



bool LogInOutManager::checkExistance()
{
    usersFile.open("../JSON/users.json");
    json usersJSON;
    usersJSON = json::parse(usersFile);
    usersFile.close();

    json usersObjs = usersJSON["users"];

    for (json::iterator it = usersObjs.begin(); it != usersObjs.end(); it++)
        {
            if((*it)["username"] == username.substr(0,strlen(username.c_str())))
                {
                    if((*it)["password"] == password.substr(0,strlen(password.c_str())))
                        {
                            return true;
                        }
                    else
                        {
                            return false;
                        }
                }
        }

    return false;

}

void LogInOutManager::addUser()
{
    usersFile.open("../JSON/users.json");
    json usersJSON;

    usersJSON = json::parse(usersFile);

    usersFile.close();

    json userAdd;
    userAdd["username"] = username.c_str();
    userAdd["password"] = password.c_str();

    usersJSON["users"].push_back(userAdd);

    usersFile.open("../JSON/users.json");

    usersFile << setw(4) << usersJSON;

    usersFile.close();

}

LogInOutManager::LogInOutManager(string username, string password, int sockDescriptor) : username(username), password(password)
{
    commObj.setDescriptor(sockDescriptor);
}

bool LogInOutManager::LogIn()
{
    username = commObj.recvStringFromDescriptor();
    password = commObj.recvStringFromDescriptor();

    // Log In

    if (checkExistance())
        {
            // aduaga clientul la active users
            pthread_mutex_lock(&clientsMutex);

            bool isFull = true;
            for(int i = 0; i < MAX_ACTIVE_USERS; i++)
                {
                    if(activeUsers[i] == "")
                        {
                            activeUsers[i] = username;
                            activeUserID = i;
                            isFull = false;
                            break;
                        }
                }

            if(isFull)
                {
                    pthread_mutex_unlock(&clientsMutex);
                    commObj.sendStringToDescriptor("SERVER IS FULL!");
                    return false;
                }
            else
                {
                    pthread_mutex_unlock(&clientsMutex);
                    commObj.sendStringToDescriptor("SUCCESS");
                    return true;
                }

        }
    else
        {
            commObj.sendStringToDescriptor("INVALID COMBINATION!");
            return false;
        }


}



bool LogInOutManager::Register()
{
    username = commObj.recvStringFromDescriptor();
    password = commObj.recvStringFromDescriptor();

    // Register

    if (checkExistance())
        {
            commObj.sendStringToDescriptor("ALREADY EXISTS!");
            return false;
        }
    else
        {
            addUser();
            commObj.sendStringToDescriptor("SUCCESS");
            return true;
        }


}

string LogInOutManager::getUserName()
{
    return username;
}

int LogInOutManager::getActiveUsrID()
{
    return activeUserID;
}



bool LogInOutManager::LogOut()
{

    pthread_mutex_lock(&clientsMutex);

    activeUsers[activeUserID] = "";

    pthread_mutex_unlock(&clientsMutex);

    commObj.sendStringToDescriptor("SUCCESS");

    return true;
}

