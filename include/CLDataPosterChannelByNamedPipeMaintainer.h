#ifndef CLDATAPOSTERCHANNELBYNAMEDPIPEMAINTAINER_H
#define CLDATAPOSTERCHANNELBYNAMEDPIPEMAINTAINER_H

#include "CLDataPosterChannelMaintainer.h"
#include "CLNamedPipe.h"
// #define PIPE_QUEUE_BETWEEN_PROCESS 0
// #define PIPE_QUEUE_IN_PROCESS 1


class CLDataPosterChannelByNamedPipeMaintainer : public CLDataPosterChannelMaintainer
{
public:
	CLDataPosterChannelByNamedPipeMaintainer(const char* pStrPipeName, bool isSharedPipe = false);
	// CLDataPosterChannelByNamedPipeMaintainer(CLEpollWapper *pEpollHandler, const char* pStrPipeName, bool isSharedPipe = 0);
	virtual ~CLDataPosterChannelByNamedPipeMaintainer();

public:
	virtual CLStatus Initialize(void *pContext);
	virtual CLStatus UnInitialize(void *pContext);
	virtual CLDataPoster* GetDataPoster();

private:
	CLNamedPipe *m_pNamedPipe;
	// CLDataPoster *m_pDataPoster;
	// CLProtocolDataPoster *m_pProtoDataPoster;
	// CLEpollWapper *m_pEpollHandler;
};

#endif