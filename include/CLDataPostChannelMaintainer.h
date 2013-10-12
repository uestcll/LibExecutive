#ifndef CLDataPostChannelMaintainer_H
#define CLDataPostChannelMaintainer_H

#include "CLStatus.h"

class CLInitialDataPostChannelNotifier;
class CLDataPoster;

class CLDataPostChannelMaintainer
{
public:
	explicit CLDataPostChannelMaintainer(CLInitialDataPostChannelNotifier *pNotifier);
	virtual ~CLDataPostChannelMaintainer();

	virtual CLStatus Initialize(void *pContext) = 0;
	virtual CLStatus Uninitialize(void *pContext) = 0;

	virtual CLDataPoster* GetDataPoster() = 0;

private:
	CLDataPostChannelMaintainer(const CLDataPostChannelMaintainer&);
	CLDataPostChannelMaintainer& operator=(const CLDataPostChannelMaintainer&);

protected:
	CLInitialDataPostChannelNotifier *m_pNotifier;
};

#endif