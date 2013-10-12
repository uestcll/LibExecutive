#ifndef CLDataPostChannelBySTLqueueMaintainer_H
#define CLDataPostChannelBySTLqueueMaintainer_H

#include "CLDataPostChannelMaintainer.h"

class CLSTLqueue;

class CLDataPostChannelBySTLqueueMaintainer : public CLDataPostChannelMaintainer
{
public:
	explicit CLDataPostChannelBySTLqueueMaintainer(CLSTLqueue *pSTLqueue);
	virtual ~CLDataPostChannelBySTLqueueMaintainer();

	virtual CLStatus Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext);
	virtual CLStatus Uninitialize(void *pContext);

	virtual CLDataPoster* GetDataPoster();

private:
	CLDataPostChannelBySTLqueueMaintainer(const CLDataPostChannelBySTLqueueMaintainer&);
	CLDataPostChannelBySTLqueueMaintainer& operator=(const CLDataPostChannelBySTLqueueMaintainer&);

private:
	CLSTLqueue *m_pSTLqueue;
};

#endif