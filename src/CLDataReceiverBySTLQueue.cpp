#include "CLDataReceiverBySTLQueue.h"
#include "CLLogger.h"
#include "CLSTLQueue.h"
#include "CLMessage.h"
#include "CLIOVector.h"

CLDataReceiverBySTLQueue::CLDataReceiverBySTLQueue(CLSTLQueue* pDataSTLQueue)
{
    m_pDataSTLQueue = pDataSTLQueue;
}

CLDataReceiverBySTLQueue::~CLDataReceiverBySTLQueue()
{
   if(m_pDataSTLQueue)
   {
       delete m_pDataSTLQueue;
       m_pDataSTLQueue = NULL;
   }
}

// CLStatus CLDataReceiverBySTLQueue::GetData(CLBuffer *pBuffer)
CLStatus CLDataReceiverBySTLQueue::GetData(CLIOVector &IOVec)
{
  int index = 0;

  while(1)
  {
      CLMessage *pMsg = m_pDataSTLQueue->PopMessage();
      if(pMsg == 0)
          break; 

      if(IOVec.Length() < (index + sizeof(CLMessage *)))
          return CLStatus(index, 0);
      
      CLStatus s = IOVec.WriteData((char*)&pMsg, index, sizeof(CLMessage*));//in pbuffer->writedata the usedlen is added ,so here dont need to return the added len in status
      
      if(!s.IsSuccess())
      {
          CLLogger::WriteLogMsg("In CLDataReceiverBySTLQueue::GetData(), pBuffer WriteData error", 0);
          return s;
      }

      index += sizeof(CLMessage *);
  }

  return CLStatus(index, 0);
} 
