#include "commandmode.h"


extern string activeUsers[10000];
extern pthread_mutex_t clientsMutex;

extern int MAX_ACTIVE_USERS;
extern int MAX_ACTIVE_SESSIONS;

extern ActiveSession activeSessions[5000];
extern pthread_t sessionThreads[5000];
extern pthread_mutex_t sessionMutex[5000];

extern string filesOpenned[5000];



CommandMode::CommandMode(int clientDescriptor) : clientDescriptor(clientDescriptor)
{
    commObj.setDescriptor(clientDescriptor);
}


void CommandMode::activateCMMode()
{

    while (1)
        {
            // CITIM DE LA CLIENT
            string messageFromClient = commObj.recvStringFromDescriptor();

            processCommand(messageFromClient);

        }
}


void CommandMode::processCommand(string messageFromClient)
{

    switch (convertToEnum(messageFromClient))
        {
        case createF:
        {

            commObj.sendStringToDescriptor("Ready");

            fileName = commObj.recvStringFromDescriptor();

            if(fileName.empty())
                break;

            pthread_mutex_lock(&clientsMutex);
            while(1)
                {
                    if(fm.createFile(fileName, username))
                        {
                            commObj.sendStringToDescriptor("Done!");
                            break;
                        }
                    else
                        {
                            commObj.sendStringToDescriptor("Exists");
                            fileName = commObj.recvStringFromDescriptor();

                            if(fileName.empty())
                                break;
                        }
                }
            pthread_mutex_unlock(&clientsMutex);
        }

        break;

        case permissionF:
        {
            commObj.sendStringToDescriptor("Ready");
            string permUsername = commObj.recvStringFromDescriptor();
            string numOfFilesStr = commObj.recvStringFromDescriptor();
            int numOfFiles = std::stoi(numOfFilesStr);


            pthread_mutex_lock(&clientsMutex);
            while(numOfFiles > 0)
            {
                fileName = commObj.recvStringFromDescriptor();

                bool fileExists = fm.findFile(fileName);

                //VERIFICA PERMISIUNI
                deque<string> adminsCheck = fm.getAdmins();
                bool userHasPermission = false;
                if(std::find(adminsCheck.begin(), adminsCheck.end(), username) != adminsCheck.end())
                    userHasPermission = true;

                if(fileExists && userHasPermission && fm.addPermission(fileName, permUsername))
                    {

                        commObj.sendStringToDescriptor("Done!");
                    }
                else
                    {
                        commObj.sendStringToDescriptor("Error");
                    }

                --numOfFiles;
            }
            pthread_mutex_unlock(&clientsMutex);


        }

        break;

        case downloadF:
        {
            commObj.sendStringToDescriptor("Ready");

            string numOfFilesStr = commObj.recvStringFromDescriptor();
            int numOfFiles = std::stoi(numOfFilesStr);

            pthread_mutex_lock(&clientsMutex);
            while(numOfFiles > 0)
                {
                    fileName = commObj.recvStringFromDescriptor();

                    if(fm.findFile(fileName))
                        {
                            commObj.sendStringToDescriptor("Done!");
                            string fileContent = fm.downlodFile(fileName);
                            commObj.sendStringToDescriptor(fileContent);
                        }
                    else
                        {
                            commObj.sendStringToDescriptor("Error");
                        }

                    --numOfFiles;
                }
            pthread_mutex_unlock(&clientsMutex);
        }

        break;

        case openF:
        {

            fileName = commObj.recvStringFromDescriptor();

            int sessionId, clientId;

            pthread_mutex_lock(&clientsMutex);
            bool isFileOpened = false;
            for(int i = 0; i < MAX_ACTIVE_SESSIONS; i++)
                {
                    if(filesOpenned[i] == fileName)
                        {
                            isFileOpened = true;

                            sessionId = i;

                            clientId = activeSessions[sessionId].addUser(username);


                            // verificam daca este loc in sesiune
                            if(clientId >= 0)
                                {
                                    commObj.sendStringToDescriptor("SUCCESS");

                                    string fileContent = fm.downlodFile(fileName);
                                    commObj.sendStringToDescriptor(fileContent);

                                    EditMode edM;
                                    edM.setClientDescriptor(clientDescriptor);
                                    edM.setClientId(clientId);
                                    edM.setSession(&(activeSessions[sessionId]));

                                    pthread_mutex_unlock(&clientsMutex);
                                    edM.activateEMode();

                                }
                            else
                                {
                                    pthread_mutex_unlock(&clientsMutex);
                                    commObj.sendStringToDescriptor("FULL");
                                }

                            break;
                        }

                }


            if(!isFileOpened)
                {
                    // trebuie creata o sesiune noua

                    bool sessionsFull = true;
                    for(int i = 0; i < MAX_ACTIVE_SESSIONS; i++)
                        {
                            if(activeSessions[i].isClear())
                                {
                                    sessionId = i;
                                    sessionsFull=false;
                                    activeSessions[sessionId].setFileName(fileName);
                                    clientId = activeSessions[sessionId].addUser(username);

                                    filesOpenned[sessionId] = fileName;

                                    int error;
                                    error = pthread_create(&(sessionThreads[sessionId]), nullptr, createSessionThread, &sessionId);
                                    if (error != 0)
                                        {

                                            commObj.sendStringToDescriptor("CANNOT CREATE THREAD!");

                                            perror("[SERVER] | [ERROR] : THREAD CAN'T BE CREATED :\n");
                                            perror(strerror(error));
                                            pthread_mutex_unlock(&clientsMutex);
                                        }
                                    else
                                        {
                                            commObj.sendStringToDescriptor("SUCCESS");

                                            string fileContent = fm.downlodFile(fileName);
                                            commObj.sendStringToDescriptor(fileContent);

                                            EditMode edM;
                                            edM.setClientId(clientId);
                                            edM.setClientDescriptor(clientDescriptor);
                                            edM.setSession(&(activeSessions[sessionId]));
                                            pthread_mutex_unlock(&clientsMutex);

                                            edM.activateEMode();
                                        }

                                    break;
                                }

                        }

                    if(sessionsFull)
                        {
                            pthread_mutex_unlock(&clientsMutex);
                            commObj.sendStringToDescriptor("SESSIONS FULL!");
                            perror("[SERVER] | [ERROR] : SESSIONS FULL :\n");

                        }

                }

        }


        break;

        case deleteF:
        {
            commObj.sendStringToDescriptor("Ready");
            string numOfFilesStr = commObj.recvStringFromDescriptor();
            int numOfFiles = std::stoi(numOfFilesStr);

            pthread_mutex_lock(&clientsMutex);
            while(numOfFiles > 0)
                {
                    fileName = commObj.recvStringFromDescriptor();

                    bool fileExists = fm.findFile(fileName);

                    //VERIFICA PERMISIUNI
                    deque<string> adminsCheck = fm.getAdmins();
                    bool userHasPermission = false;
                    if(std::find(adminsCheck.begin(), adminsCheck.end(), username) != adminsCheck.end())
                        userHasPermission = true;


                    if(fileExists && userHasPermission && fm.deleteFile(fileName))
                        {
                            commObj.sendStringToDescriptor("Done!");
                        }
                    else
                        {
                            commObj.sendStringToDescriptor("Error");
                        }

                    --numOfFiles;
                }
            pthread_mutex_unlock(&clientsMutex);

        }

        break;


        case loginF:
        {

            if(log.LogIn())
                {
                    username = log.getUserName();
                    activeUserID = log.getActiveUsrID();

                    FileManager tempFm;
                    deque<string> files = tempFm.getAllUserFiles(username);
                    for(auto it : files)
                        {
                            commObj.sendStringToDescriptor(it);
                        }
                    commObj.sendStringToDescriptor("ALL FILES SENT!");

                    loggedIn = true;
                    cout << "LOGIN SUCCESSFUL" << endl;

                }


        }

        break;

        case logoutF:
        {
            if(log.LogOut())
                {
                    loggedIn = false;
                    cout << "LOGOUT SUCCESSFUL" << endl;
                    username.clear();
                    activeUserID = -1;



                }


        }

        break;

        case quitF:
        {
            commObj.sendStringToDescriptor("quit");
            close(clientDescriptor);
            cout << "[SERVER] : CLIENT DISCONNECTED!" << endl;

            if(loggedIn)
                {
                    pthread_mutex_lock(&clientsMutex);

                    activeUsers[activeUserID] = "";

                    pthread_mutex_unlock(&clientsMutex);
                    activeUserID = -1;
                }

            loggedIn = false;
            pthread_exit(nullptr);
        }

        break;

        case registerF:
        {
            if(log.Register())
                cout << "REGISTER SUCCESSFUL" << endl;
        }

        break;

        default:
            cout << "There is no reference to that command!" << endl;

        }

}

AvailableCommands CommandMode::convertToEnum(string message)
{

    if (message.substr(0, 4) == "open")
        return openF;
    else if (message.substr(0,6) == "create")
        return createF;
    else if (message.substr(0,11) == "permissions")
        return permissionF;
    else if (message.substr(0, 8) == "download")
        return downloadF;
    else if (message.substr(0, 6) == "delete")
        return deleteF;
    else if (message.substr(0,4) == "quit")
        return quitF;
    else if (message.substr(0,5) == "login")
        return loginF;
    else if (message.substr(0,6) == "logout")
        return logoutF;
    else if (message.substr(0,8) == "register")
        return registerF;
    else
        return errorF;
}

