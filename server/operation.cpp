#include "operation.h"



Operation::Operation() : type(DEFAULT), noChars(0), position(0), charsInserted("")//, charsRemoved("")
{

}

Operation::Operation(TypeOfOp type, int noChars, int position, string &chars) : type(type), noChars(noChars), position(position)
{
    charsInserted = chars;
}


void Operation::setType(TypeOfOp operationType)
{
    type = operationType;
}

void Operation::setNoChars(int operationNoChars)
{
    noChars = operationNoChars;
}

void Operation::setPosition(int operationPosition)
{
    position = operationPosition;
}

void Operation::setCharsInserted(string inserted)
{
    charsInserted = inserted;
}


TypeOfOp Operation::getType()
{
    return type;
}

int Operation::getNoChars()
{
    return noChars;
}

int Operation::getPosition()
{
    return position;
}

string Operation::getCharsInserted()
{
    return charsInserted;
}
