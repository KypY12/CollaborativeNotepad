#ifndef LOGINOUTMANAGER_H
#define LOGINOUTMANAGER_H

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "communication.h"
#include "nlohmann/json.hpp"



using json = nlohmann::json;


using namespace std;


class LogInOutManager
{

private:
    fstream usersFile;

    Communication commObj;
    string username;
    string password;

    int activeUserID;

    bool checkExistance();

    void addUser();

public:
    LogInOutManager() = delete;
    LogInOutManager(string username, string password, int sockDescriptor);

    bool LogIn();

    bool LogOut();

    bool Register();

    string getUserName();
    int getActiveUsrID();



};

#endif // LOGINOUTMANAGER_H
