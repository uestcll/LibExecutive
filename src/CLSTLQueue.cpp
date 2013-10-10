#include "CLSTLQueue.h"
#include "CLMessage.h"
#include "CLCriticalSection.h"
#include "CLMessage.h"
#include "CLLogger.h"

CLSTLQueue::CLSTLQueue()
{

}

CLSTLQueue::~CLSTLQueue()
{
    while(1)
    {
        CLMessage* p = PopMessage();
        if(p == NULL)
            break;
        
        delete p;
    }
}

CLMessage* CLSTLQueue::PopMessage()
{
    try
    {
        CLCriticalSection cs(&m_MutexForQueue);
        if(m_MsgQueue.empty())
            return 0;
        CLMessage* p = m_MsgQueue.front();
        m_MsgQueue.pop();

        return p;
    }
    catch(const char* str)
    {
        CLLogger::WriteLogMsg("In CLSTLQueue::PopMessage() error", 0);
        return 0;
    }
}

CLStatus CLSTLQueue::PushMessage(CLMessage* pMsg)
{
    try
    {
        CLCriticalSection cs(&m_MutexForQueue);

        m_MsgQueue.push(pMsg);

    }
    catch(const char* str)
    {
        CLLogger::WriteLogMsg("In CLSTLQueue::PushMessage() error", 0);
        delete pMsg;
        return CLStatus(-1, 0);
    }

    return CLStatus(0, 0);
}