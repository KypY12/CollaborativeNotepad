#include "communication.h"


Communication::Communication() : commDescriptor(0)
{

}

Communication::Communication(int descriptor) : commDescriptor(descriptor)
{

}

void Communication::sendStringToDescriptor(string message)
{
    char commBuffer[commBufferSize];

    int segmentsCount = message.length() / commBufferSize + 1;
    for (int segmentIndex = 0; segmentIndex < segmentsCount; segmentIndex++)    // Trimitem mesajul bucata cu bucata
        {
            bzero(commBuffer, commBufferSize);
            strncpy(commBuffer, (message.substr(segmentIndex * commBufferSize, (segmentIndex + 1) * commBufferSize)).c_str(), commBufferSize);
            if (write(commDescriptor, commBuffer, commBufferSize) <= 0) // Trimitem o parte din mesaj
                {
                    perror("[ERROR] : CANNOT SEND DATA!\n");
                    perror(strerror(errno));
                    return;
                }
        }

    if (write(commDescriptor, "\0", commBufferSize) <= 0)  // Caracterul null trimis ii va spune celuilalt capat sa se opreasca din citit
        {
            perror("[ERROR] : CANNOT SEND DATA!\n");
            perror(strerror(errno));
            return;
        }

}

string Communication::recvStringFromDescriptor()
{
    string message;
    char commBuffer[commBufferSize];

    while (1)
        {
            bzero(commBuffer, commBufferSize);

            if (read(commDescriptor, commBuffer, commBufferSize) <= 0)
                {
                    perror("[ERROR] : CANNOT READ DATA FROM CLIENT !\n");
                    perror(strerror(errno));
                    message.clear();
                    return message;
                }
            if (strlen(commBuffer) > 0)
                {
                    message.append(commBuffer, commBufferSize);
                }
            else
                {
                    cerr << "[OK] : MESSAGE RECEIVED !" << endl;
                    fflush(stdout);
                    break;
                }
        }

    return message.substr(0, strlen(message.c_str()));
}

void Communication::sendOperationToDescriptor(Operation &operation)
{

    TypeOfOp sendingType = operation.getType();
    int sendingNoChars = operation.getNoChars();
    int sendingPosition = operation.getPosition();


    if (write(commDescriptor, &sendingType, sizeof (TypeOfOp)) <= 0)  // Trimitem tipul
        {
            perror("[ERROR] : CANNOT SEND DATA!\n");
            perror(strerror(errno));
            return;
        }

    if (write(commDescriptor, &sendingNoChars, sizeof (int)) <= 0)  // Trimitem numarul de caractere
        {
            perror("[ERROR] : CANNOT SEND DATA!\n");
            perror(strerror(errno));
            return;
        }

    if (write(commDescriptor, &sendingPosition, sizeof (int)) <= 0)  // Trimitem pozitia
        {
            perror("[ERROR] : CANNOT SEND DATA!\n");
            perror(strerror(errno));
            return;
        }

    sendStringToDescriptor(operation.getCharsInserted());

}

Operation* Communication::recvOperationFromDescriptor()
{
    Operation *op = new Operation;

    TypeOfOp recvType;
    int recvNoChars;
    int recvPosition;


    if (read(commDescriptor, &recvType, sizeof (TypeOfOp)) <= 0)  // Trimitem tipul
        {
            perror("[ERROR] : CANNOT SEND DATA!\n");
            perror(strerror(errno));
            return op;
        }

    if (read(commDescriptor, &recvNoChars, sizeof (int)) <= 0)  // Trimitem numarul de caractere
        {
            perror("[ERROR] : CANNOT SEND DATA!\n");
            perror(strerror(errno));
            return op;
        }

    if (read(commDescriptor, &recvPosition, sizeof (int)) <= 0)  // Trimitem pozitia
        {
            perror("[ERROR] : CANNOT SEND DATA!\n");
            perror(strerror(errno));
            return op;
        }

    string recvCharsRaw = recvStringFromDescriptor();

    string recvChars = recvCharsRaw.substr(0,strlen(recvCharsRaw.c_str()));


    op->setType(recvType);
    op->setNoChars(recvNoChars);
    op->setPosition(recvPosition);
    op->setCharsInserted(recvChars);

    return op;
}

bool Communication::isMessageReceived()
{
    char buff;
    if(read(commDescriptor,&buff,1) == EAGAIN)
        {
            return true;
        }
    else
        {
            return false;
        }

}

void Communication::sendByte(char byteValue)
{
    if (write(commDescriptor, &byteValue, 1) <= 0)
        {
            perror("[ERROR] : CANNOT SEND DATA!\n");
            perror(strerror(errno));
            return;
        }

}

void Communication::setDescriptor(int descriptor)
{
    commDescriptor = descriptor;
}
