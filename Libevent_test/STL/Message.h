#ifndef MESSAGE_H
#define MESSAGE_H

#include "LibExecutive.h"
#include <iostream>
#include <cstring>

#define ADD_MESSAGE 1
#define QUIT_MESSAGE 2

class CLAddMessage : public CLMessage
{
public:
    CLAddMessage(int num1, int num2) : CLMessage(ADD_MESSAGE), m_num1(0), m_num2(0)
    {
        m_num1 = num1;
        m_num2 = num2;
    }

public:
    int m_num1;
    int m_num2;
};

class CLQuitMessage : public CLMessage
{
public:
    CLQuitMessage() : CLMessage(QUIT_MESSAGE)
    {
    }
};

#endif
