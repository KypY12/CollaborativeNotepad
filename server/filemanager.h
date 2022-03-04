#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <deque>
#include <string>
#include <fstream>
#include <iomanip>

#include <QString>
#include <QStringBuilder>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "communication.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

using namespace std;

class FileManager
{
private:
    fstream docsFile;
    fstream permissionsFile;
    fstream usersFileStream;

    json filesJSON;
    json currentFileJSON;
    json::iterator currIterator;

    string emptyStr = "";

    string fileName;
    deque<string> admins;
    deque<string> permitted;
    string fileContent;

    void createFileOnDisk();
    void deleteFileFromDisk();

    deque<string> userFiles;

public:
    FileManager();

    bool fileExists(string fileNameParam);
    bool findFile(string fileNameParam);
    void clearInfos();

    bool createFile(string fileNameParam, string creatorUsername);
    bool deleteFile(string fileNameParam);
    string & downlodFile(string fileNameParam);


    bool addPermission(string filename, string userToAdd);

    string getFileName();
    deque<string>& getAdmins();
    deque<string>& getPermitted();

    deque<string>& getAllUserFiles(string username);


};

#endif // FILEMANAGER_H
