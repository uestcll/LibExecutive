#ifndef CLDATAPOSTERCHANNELMAINTAINER_H
#define CLDATAPOSTERCHANNELMAINTAINER_H

#include "CLStatus.h"
#include "CLIOVector.h"

class CLDataPoster;

class CLDataPosterChannelMaintainer
{
public:
	CLDataPosterChannelMaintainer();
	virtual ~CLDataPosterChannelMaintainer();

public:
	virtual CLStatus Initialize(void *pContext) = 0;
	virtual CLStatus UnInitialize(void *pContext) = 0;
	virtual CLDataPoster* GetDataPoster() = 0;

private:
    CLDataPosterChannelMaintainer(const CLDataPosterChannelMaintainer&);
    CLDataPosterChannelMaintainer& operator=(const CLDataPosterChannelMaintainer&);
};


#endif