#ifndef CLDataPostChannelMaintainer_H
#define CLDataPostChannelMaintainer_H

#include "CLStatus.h"

class CLInitialDataPostChannelNotifier;
class CLDataPoster;

class CLDataPostChannelMaintainer
{
public:
	CLDataPostChannelMaintainer();
	virtual ~CLDataPostChannelMaintainer();

	virtual CLStatus Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext) = 0;
	virtual CLStatus Uninitialize(void *pContext) = 0;

	virtual CLDataPoster* GetDataPoster() = 0;

private:
	CLDataPostChannelMaintainer(const CLDataPostChannelMaintainer&);
	CLDataPostChannelMaintainer& operator=(const CLDataPostChannelMaintainer&);
};

#endif