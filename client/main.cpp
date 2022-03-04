#include "commandmodewindow.h"
#include "editmode.h"
#include "loginwindow.h"
#include <QApplication>

#include <iostream>
#include <string>
#include <fstream>

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

using namespace std;


string SERVER_IP = "127.0.0.1";
unsigned int PORT = 2024;

int main(int argc, char *argv[])
{
    int sockDescriptor;
    struct sockaddr_in serverAddr;

    if ((sockDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("[CLIENT] | [ERROR] : socket()\n");
            return errno;
        }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());
    serverAddr.sin_port = htons(PORT);

    if (connect(sockDescriptor, (struct sockaddr *)&serverAddr, (socklen_t)sizeof(struct sockaddr)) == -1)
        {
            perror("[CLIENT] | [ERROR] : connect()\n");
            return errno;
        }

    string messageToServer;



    QApplication a(argc, argv);

    LoginWindow loginWindow(nullptr, sockDescriptor);
    CommandModeWindow cmdWindow(nullptr, sockDescriptor);
    EditMode editWindow(nullptr, sockDescriptor);


    loginWindow.setCmdWindow(&cmdWindow);
    cmdWindow.setLoginWindow(&loginWindow);
    cmdWindow.setEditWindow(&editWindow);
    editWindow.setCmdWindow(&cmdWindow);

    loginWindow.show();




    return a.exec();

}
