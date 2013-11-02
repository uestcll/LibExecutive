#include "CLDataReceiverBySTLQueue.h"
#include "CLLogger.h"
#include "CLSTLQueue.h"
#include "CLMessage.h"
#include "CLBuffer.h"

CLDataReceiverBySTLQueue::CLDataReceiverBySTLQueue(CLSTLQueue* pDataSTLQueue)
{
    m_pDataSTLQueue = pDataSTLQueue;
}

CLDataReceiverBySTLQueue::~CLDataReceiverBySTLQueue()
{
//    if(m_pDataSTLQueue)
  //  {
    //    delete m_pDataSTLQueue;
      //  m_pDataSTLQueue = NULL;
   // }/
}

CLStatus CLDataReceiverBySTLQueue::GetData(CLBuffer *pBuffer)
{
	while(1)
    {
        CLMessage *pMsg = m_pDataSTLQueue->PopMessage();
        if(pMsg == 0)
            break;
  
        CLStatus s = pBuffer->WriteData((char*)&pMsg, sizeof(CLMessage*));//in pbuffer->writedata the usedlen is added ,so here dont need to return the added len in status
        if(!s.IsSuccess())
        {
            CLLogger::WriteLogMsg("In CLDataReceiverBySTLQueue::GetData(), pBuffer WriteData error", 0);
            return s;
        }
    }
    return CLStatus(0, 0);
} 
