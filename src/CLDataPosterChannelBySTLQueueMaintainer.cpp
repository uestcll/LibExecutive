#include "CLDataPosterChannelBySTLQueueMaintainer.h"
#include "CLDataPosterBySTLQueue.h"
#include "CLSTLQueue.h"
#include "CLEvent.h"
#include "CLLogger.h"
#include "CLDataPoster.h"
/*
deal with the queue free
*/
CLDataPosterChannelBySTLQueueMaintainer::CLDataPosterChannelBySTLQueueMaintainer(CLSTLQueue *pQueue)
{
	if(pQueue == 0)
		throw "In CLDataPosterChannelBySTLQueueMaintainer::CLDataPosterChannelBySTLQueueMaintainer(), para error";
	m_pQueue = pQueue;
}

CLDataPosterChannelBySTLQueueMaintainer::~CLDataPosterChannelBySTLQueueMaintainer()
{
	if(m_pQueue)
	{
		delete m_pQueue;
		m_pQueue = NULL;
	}
}

CLStatus CLDataPosterChannelBySTLQueueMaintainer::Initialize(void *pContext)
{

	return CLStatus(0, 0);
}

CLStatus CLDataPosterChannelBySTLQueueMaintainer::UnInitialize(void *pContext)
{

	return CLStatus(0, 0);
}

CLDataPoster* CLDataPosterChannelBySTLQueueMaintainer::GetDataPoster()
{
	return (new CLDataPosterBySTLQueue(m_pQueue));
}
