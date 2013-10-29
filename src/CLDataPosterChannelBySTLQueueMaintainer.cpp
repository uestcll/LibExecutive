#include "CLDataPosterChannelBySTLQueueMaintainer.h"
#include "CLDataPosterBySTLQueue.h"
#include "CLSTLQueue.h"
#include "CLEvent.h"
#include "CLLogger.h"

CLDataPosterChannelBySTLQueueMaintainer::CLDataPosterChannelBySTLQueueMaintainer(CLSTLQueue *pQueue, CLEvent *pEvent)
{
	if(pQueue == 0 || pEvent == 0)
		throw "In CLDataPosterChannelBySTLQueueMaintainer::CLDataPosterChannelBySTLQueueMaintainer(), para error";
	m_pQueue = pQueue;
	m_pEvent = pEvent;
}

CLDataPosterChannelBySTLQueueMaintainer::~CLDataPosterChannelBySTLQueueMaintainer()
{
	if(m_pQueue != 0)
	{
		delete m_pQueue;
		m_pQueue = 0;
	}
	if(m_pEvent != 0)
	{
		delete m_pEvent;
		m_pEvent = 0;
	}
	if(m_pDataPoster != 0)
	{
		delete m_pDataPoster;
		m_pDataPoster = 0;
	}
}

CLStatus CLDataPosterChannelBySTLQueueMaintainer::Initialize(void *pContext)
{
	m_pDataPoster = new CLDataPosterBySTLQueue(m_pQueue);

	return CLStatus(0, 0);
}

CLStatus CLDataPosterChannelBySTLQueueMaintainer::UnInitialize(void *pContext)
{
	if(m_pDataPoster != 0)
	{
		delete m_pDataPoster;
		m_pDataPoster = 0;
	}

	return CLStatus(0, 0);
}

CLStatus CLDataPosterChannelBySTLQueueMaintainer::PostData(CLIOVector &dataVec)
{
	CLStatus s = m_pDataPoster->PostData(dataVec);

	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterChannelBySTLQueueMaintainer::PostData(), m_pDataPoster->PostData() error", 0);
		return CLStatus(-1, 0);
	}

	if(POST_DATA_COMPLETE == s.m_clErrorCode)
	{
		CLStatus s1 = m_pEvent->set();
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLDataPosterChannelBySTLQueueMaintainer::PostData(), m_pEvent->set() error", 0);
			return CLStatus(-1, 0);
		}
	}

	return CLStatus(0, 0);
}