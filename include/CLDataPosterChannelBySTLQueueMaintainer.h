#ifndef CLDATAPOSTERCHANNELBYSTLQUEUEMAINTAINER_H
#define CLDATAPOSTERCHANNELBYSTLQUEUEMAINTAINER_H

#include "CLDataPosterChannelMaintainer.h"

class CLSTLQueue;
class CLEvent;
class CLDataPoster;

class CLDataPosterChannelBySTLQueueMaintainer : public CLDataPosterChannelMaintainer
{
public:
	CLDataPosterChannelBySTLQueueMaintainer(CLSTLQueue *pQueue, CLEvent *pEvent);
	virtual ~CLDataPosterChannelBySTLQueueMaintainer();

public:
	virtual CLStatus Initialize(void *pContext);
	virtual CLStatus UnInitialize(void *pContext);
	virtual CLStatus PostData(CLIOVector &dataVec);

private:
	CLSTLQueue *m_pQueue;
	CLEvent *m_pEvent;
	CLDataPoster *m_pDataPoster;
};


#endif