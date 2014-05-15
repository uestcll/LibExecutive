#include "LibExecutive.h"
#include "Message.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>

using namespace std;

class CLSTLMessageObserver : public CLMessageObserver
{
public:
    CLSTLMessageObserver()
    {}
    ~CLSTLMessageObserver()
    {}

    virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void *pContext)
    {
        pMessageLoop->Register(ADD_MESSAGE, (CallBackForMessageLoop)(&CLSTLMessageObserver::On_Add_Message));
        pMessageLoop->Register(QUIT_MESSAGE, (CallBackForMessageLoop)(&CLSTLMessageObserver::On_Quit_Message));
        return CLStatus(0, 0);
    }

    CLStatus On_Add_Message(CLMessage *pMessage)
    {
        CLAddMessage *pAdd = dynamic_cast<CLAddMessage*>(pMessage);
        int num1 = pAdd->m_num1;
        int num2 = pAdd->m_num2;
        cout << "AddMessage : " << num1 << " + " << num2 << " = " << num1 + num2 << endl;

        return CLStatus(0, 0);
    }

    CLStatus On_Quit_Message(CLMessage *pMessage)
    {
        return CLStatus(QUIT_MESSAGE_LOOP, 0);
    }
};

class CLThreadFunction : public CLExecutiveFunctionProvider
{
public:
    CLThreadFunction()
    {
    }
    virtual ~CLThreadFunction()
    {
    }


    virtual CLStatus RunExecutiveFunction(void *pContext)
    {
        CLAddMessage *addmessage1 = new CLAddMessage(1, 2);
        CLAddMessage *addmessage2 = new CLAddMessage(19, 11);
        CLQuitMessage *quitmessage = new CLQuitMessage;

        CLExecutiveNameServer::PostExecutiveMessage("mainThread", addmessage1);
        CLExecutiveNameServer::PostExecutiveMessage("mainThread", addmessage2);
        CLExecutiveNameServer::PostExecutiveMessage("mainThread", quitmessage);

        return CLStatus(0, 0);
    }
};

int main(int argc, char *argv[])
{
    if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
    {
        CLLogger::WriteLogMsg("CLLibExecutiveInitializer initialize error", -1);
        return 0;
    }

    CLSTLMessageObserver *myObserver = new CLSTLMessageObserver;

    CLNonThreadForMsgLoop mainThread(myObserver, "mainThread", EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);

    CLThread *myThread = new CLThread(new CLThreadFunction(), true);

    myThread->Run(0);
    mainThread.Run(0);
    myThread->WaitForDeath();

    delete myThread;

    if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
    {
        CLLogger::WriteLogMsg("CLLibExecutiveInitializer destroy error", -1);
    }

    return 0;
}
