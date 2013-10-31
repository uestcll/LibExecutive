#include "CLDataPosterBySTLQueue.h"
#include "CLLogger.h"
#include "CLMessage.h"

CLDataPosterBySTLQueue::CLDataPosterBySTLQueue(CLSTLQueue *pQueue)
{ 
	m_pQueue = pQueue;
}

CLDataPosterBySTLQueue::~CLDataPosterBySTLQueue()
{

}

CLStatus CLDataPosterBySTLQueue::PostData(CLIOVector* pDataVec) //do not deal with the free of data vec
{
	CLMessage *pMsg;
	
	int msgLen = sizeof(CLMessage*);

	if(pDataVec->Length() != msgLen)
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), pDataVec length is < msgLen", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = pDataVec->ReadData((char*)&pMsg, 0, msgLen);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), pDataVec->ReadData() error", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s1 = m_pQueue->PushMessage(pMsg);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), m_pQueue->PushMessage error", 0);
		return CLStatus(-1, 0);
	}

	// pDataVec->FreeAndPopAll();

	return CLStatus(msgLen, 0);//send msg success, and notify the channelmaitainer to delete CLDataPoster and free dataVec
}