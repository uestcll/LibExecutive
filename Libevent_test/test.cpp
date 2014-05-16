#include "LibExecutive.h"
#include "Message.h"
#include <string>
#include <iostream>

class CLLibeventObserver : public CLMessageObserver
{
public:
    CLLibeventObserver()
    {
    }
    ~CLLibeventObserver()
    {
    }

    virtual CLStatus Initialize(CLMessageLoopManager)
};
int main(int argc, char *argv[])
{

}
