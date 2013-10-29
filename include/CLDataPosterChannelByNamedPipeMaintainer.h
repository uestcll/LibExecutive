#ifndef CLDATAPOSTERCHANNELBYNAMEDPIPEMAINTAINER_H
#define CLDATAPOSTERCHANNELBYNAMEDPIPEMAINTAINER_H

#include "CLDataPosterChannelMaintainer.h"

#define PIPE_QUEUE_BETWEEN_PROCESS 0
#define PIPE_QUEUE_IN_PROCESS 1

class CLNamdePipe;
class CLEvent;
// class CLDataPoster;
class CLEpollWapper;
class CLProtocolDataPoster;

class CLDataPosterChannelByNamedPipeMaintainer : public CLDataPosterChannelMaintainer
{
public:
	CLDataPosterChannelByNamedPipeMaintainer(CLEvent *pEvent, const char* pStrPipeName, bool isSharedPipe = 0);
	CLDataPosterChannelByNamedPipeMaintainer(CLEpollWapper *pEpollHandler, const char* pStrPipeName, bool isSharedPipe = 0);
	virtual ~CLDataPosterChannelByNamedPipeMaintainer();

public:
	virtual CLStatus Initialize(CLProtocolDataPoster *pProtoDataPoster, void *pContext);
	virtual CLStatus UnInitialize(void *pContext);
	virtual CLDataPoster* GetDataPoster();

private:
	CLNamedPipe *m_pNamedPipe;
	CLEvent *m_pEvent;
	// CLDataPoster *m_pDataPoster;
	CLProtocolDataPoster *m_pProtoDataPoster;
	CLEpollWapper *m_pEpollHandler;
};

#endif