#ifndef CLDataPostChannelByNamedPipeMaintainer_H
#define CLDataPostChannelByNamedPipeMaintainer_H

#include "CLDataPostChannelMaintainer.h"
#include "CLNamedPipe.h"

class CLDataPostChannelByNamedPipeMaintainer : public CLDataPostChannelMaintainer
{
public:
	CLDataPostChannelByNamedPipeMaintainer(const char *pstrNamedPipe);
	virtual ~CLDataPostChannelByNamedPipeMaintainer();

	virtual CLStatus Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext);
	virtual CLStatus Uninitialize(void *pContext);

	virtual CLDataPoster* GetDataPoster();

private:
	CLDataPostChannelByNamedPipeMaintainer(const CLDataPostChannelByNamedPipeMaintainer&);
	CLDataPostChannelByNamedPipeMaintainer& operator=(const CLDataPostChannelByNamedPipeMaintainer&);

private:
	CLNamedPipe m_NamedPipe;
};

#endif