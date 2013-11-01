#ifndef CLDATAPOSTERCHANNELBYSTLQUEUEMAINTAINER_H
#define CLDATAPOSTERCHANNELBYSTLQUEUEMAINTAINER_H

#include "CLDataPosterChannelMaintainer.h"

class CLSTLQueue;
class CLEvent;

class CLDataPosterChannelBySTLQueueMaintainer : public CLDataPosterChannelMaintainer
{
public:
	CLDataPosterChannelBySTLQueueMaintainer(CLSTLQueue *pQueue);
	virtual ~CLDataPosterChannelBySTLQueueMaintainer();

public:
	virtual CLStatus Initialize(void *pContext);
	virtual CLStatus UnInitialize(void *pContext);
	virtual CLDataPoster* GetDataPoster();

private:
	CLSTLQueue *m_pQueue;
};


#endif