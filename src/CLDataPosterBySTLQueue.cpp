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

CLStatus CLDataPosterBySTLQueue::PostData(CLIOVector& dataVec)
{
	CLMessage *pMsg;
	
	int msgLen = sizeof(CLMessage*);

	if(dataVec.Length() < msgLen)
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), dataVec length is < msgLen", 0);
		return CLStatus(-1, POST_DATA_BUF_ERROR);
	}

	CLStatus s = dataVec.ReadData((char*)&pMsg, 0, msgLen);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), dataVec.ReadData() error", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s1 = m_pQueue->PushMessage(pMsg);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), m_pQueue->PushMessage error", 0);
		return CLStatus(-1, POST_DATA_ERROR);
	}

	dataVec.FreeAndPopAll();

	return CLStatus(sizeof(pMsg), POST_DATA_COMPLETE);//send msg success, and notify the channelmaitainer to delete CLDataPoster and free dataVec
}