#include "CLDataPosterBySTLQueue.h"
#include "CLLogger.h"
#include "CLMessage.h"

CLDataPosterBySTLQueue::CLDataPosterBySTLQueue(CLSTLQueue queue)
{
	m_Queue = queue;
}

CLDataPosterBySTLQueue::~CLDataPosterBySTLQueue()
{

}

CLStatus CLDataPosterBySTLQueue::PostData(CLIOVector dataVec)
{
	CLMessage *pMsg = new CLMessage();
	
	int msgLen = sizeof(CLMessage*);

	if(dataVec.length() < msgLen)
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), dataVec length is < msgLen", 0);
		return CLStatus(-1, 0);
	}

	CLStatus s = dataVec.ReadData((char*)&pMsg, 0, msgLen);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), dataVec.ReadData() error", 0);
		return s;
	}

	CLStatus s1 = m_Queue.PushMessage(pMsg);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterBySTLQueue::PostData(), m_Queue.PushMessage error", 0);
		return s1;
	}

	return CLStatus(sizeof(pMsg), 0);//send msg success, and notify the channelmaitainer to delete CLDataPoster and free dataVec
}