#include <gtest/gtest.h>
#include "CLExecutiveCommunication.h"
#include "CLExecutiveNameServer.h"
#include "CLStatus.h"
#include "CLMessage.h"

class CLThrowTest : public CLExecutiveCommunication
{
    public:
        CLStatus PostExecutiveMessage(CLMessage *pMessage)
        {
            throw "hello, world!";

            return CLStatus(0, 0);
        }
};

TEST(CLExecutiveNameServer, PostExecutiveMessage)
{
    CLExecutiveNameServer::Create();
    CLExecutiveNameServer *p = CLExecutiveNameServer::GetInstance();
    CLExecutiveCommunication *pC = new CLThrowTest;
    p->Register("hello", pC);

    CLMessage *pM = new CLMessage(10);
    CLStatus s = CLExecutiveNameServer::PostExecutiveMessage("hello", pM);
    
    EXPECT_TRUE(s.IsSuccess());
}

