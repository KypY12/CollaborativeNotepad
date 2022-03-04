#ifndef OPERATION_H
#define OPERATION_H

#include <iostream>
#include <string>


using namespace std;

enum TypeOfOp
{
    EXIT = 0,
    DEFAULT = 1,
    INSERT = 2,
    REMOVE = 3,
    MIXED = 4
};


class Operation
{
private:
    TypeOfOp type;
    int noChars;
    int position;
    string charsInserted;

public:

    Operation();
    Operation(TypeOfOp type, int noChars, int position, string &chars);

    void setType(TypeOfOp operationType);
    void setNoChars(int operationNoChars);
    void setPosition(int operationPosition);
    void setCharsInserted(string inserted);

    TypeOfOp getType();
    int getNoChars();
    int getPosition();

    string getCharsInserted();
};

#endif // OPERATION_H
