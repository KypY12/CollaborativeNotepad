#include "filemanager.h"

void FileManager::createFileOnDisk()
{
    fstream fileCreate;
    fileCreate.open("../files/"+fileName, ios::out);
    fileCreate << "";
    fileCreate.close();
}

void FileManager::deleteFileFromDisk()
{
    string filePath = "../files/"+fileName;
    remove(filePath.c_str());
}

FileManager::FileManager()
{

}

bool FileManager::fileExists(string fileNameParam)
{

    docsFile.open("../JSON/files.json", ios::in);
    filesJSON = json::parse(docsFile);
    docsFile.close();

    currIterator = filesJSON.find(fileNameParam);

    if(currIterator != filesJSON.end())
        {
            return true;
        }
    else
        {
            return false;
        }

}

bool FileManager::findFile(string fileNameParam)
{
    docsFile.open("../JSON/files.json", ios::in);
    filesJSON = json::parse(docsFile);
    docsFile.close();

    currIterator = filesJSON.find(fileNameParam);

    if(currIterator != filesJSON.end())
        {
            fileName = fileNameParam;

            currentFileJSON = *currIterator;

            return true;
        }
    else
        {
            return false;
        }

}

bool FileManager::createFile(string fileNameParam, string creatorUsername)
{
    clearInfos();

    if(fileExists(fileNameParam))
        {
            return false;
        }
    else
        {
            admins.push_back(creatorUsername);

            json adminsObj;
            adminsObj.push_back(creatorUsername);
            json permittedObj = json::array();
            currentFileJSON = {{"admins", adminsObj}, {"permitted", permittedObj}};

            filesJSON[fileNameParam.c_str()] = currentFileJSON;

            fileName = fileNameParam;
            createFileOnDisk();

            docsFile.open("../JSON/files.json", ios::out | ios::trunc);
            docsFile << setw(4) << filesJSON;
            docsFile.close();

            return true;
        }

}

bool FileManager::deleteFile(string fileNameParam)
{
    clearInfos();
    if(!fileExists(fileNameParam))
        {
            return false;
        }
    else
        {

            filesJSON.erase(currIterator);

            fileName = fileNameParam;
            deleteFileFromDisk();


            docsFile.open("../JSON/files.json", ios::out | ios::trunc);
            docsFile << setw(4) << filesJSON;
            docsFile.close();

            return true;
        }

}

string &FileManager::downlodFile(string fileNameParam)
{
    fileContent.clear();
    fstream file;
    file.open("../files/"+fileNameParam, ios::in);
    if(file.is_open())
    {
        string tempStr;

        while(getline(file,tempStr))
        {
            fileContent += tempStr;
            tempStr.clear();
        }
        file.close();
        return fileContent;
    }
    else
    {
        cerr << "CANNOT OPEN FILE" << endl;
        file.close();
        return fileContent;
    }

}

bool FileManager::addPermission(string filename, string userToAdd)
{

    usersFileStream.open("../JSON/users.json");
    json usersJSON;
    usersJSON = json::parse(usersFileStream);
    usersFileStream.close();

    json usersObjs = usersJSON["users"];

    bool found = false;
    for (json::iterator it = usersObjs.begin(); it != usersObjs.end(); it++)
        {
            if((*it)["username"] == userToAdd.substr(0,strlen(userToAdd.c_str())))
                {
                    found = true;
                    break;
                }
        }

    if(!found)
    {
        return false;
    }
    else
    {
        currentFileJSON["permitted"].push_back(userToAdd);
        filesJSON[filename.c_str()] = currentFileJSON;
        docsFile.open("../JSON/files.json", ios::out | ios::trunc);
        docsFile << setw(4) << filesJSON;
        docsFile.close();
        return true;
    }



}

string FileManager::getFileName()
{
    return fileName;
}

deque<string>& FileManager::getAdmins()
{

    for(auto adIt : currentFileJSON["admins"])
        {
            string tempAdm = adIt;
            admins.push_back(tempAdm);
        }
    return admins;
}

deque<string> &FileManager::getPermitted()
{
    for(auto permIt : currentFileJSON["permitted"])
        {
            string tempPerm = permIt;
            permitted.push_back(tempPerm);
        }
    return permitted;
}

deque<string> &FileManager::getAllUserFiles(string username)
{
    docsFile.open("../JSON/files.json", ios::in);
    filesJSON = json::parse(docsFile);
    docsFile.close();

    currIterator = filesJSON.begin();
    while(currIterator != filesJSON.end())
        {
            json::iterator tempIt = (*currIterator)["admins"].begin();
            while(tempIt != (*currIterator)["admins"].end())
                {
                    if(username == *tempIt)
                        {
                            userFiles.push_back(currIterator.key());
                        }
                    tempIt++;
                }

            json::iterator tempIt2 = (*currIterator)["permitted"].begin();
            while(tempIt2 != (*currIterator)["permitted"].end())
                {
                    if(username == *tempIt2)
                        {
                            userFiles.push_back(currIterator.key());
                        }
                    tempIt2++;
                }
            currIterator++;
        }

    return userFiles;
}

void FileManager::clearInfos()
{
    filesJSON.clear();
    currentFileJSON.clear();
    fileName.clear();
    admins.clear();
    permitted.clear();
    fileContent.clear();
}
