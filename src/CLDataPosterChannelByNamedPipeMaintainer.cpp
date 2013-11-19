#include "CLDataPosterChannelByNamedPipeMaintainer.h"
#include "CLLogger.h"
#include "CLProtocolDataPoster.h"
#include "CLDataPosterByNamedPipe.h"
#include "CLMessage.h"
/*
deal with the namedpipe free
*/
CLDataPosterChannelByNamedPipeMaintainer::CLDataPosterChannelByNamedPipeMaintainer(const char* pStrPipeName, bool isSharedPipe) : m_bIsSharedPipe(isSharedPipe)
{
	if(!pStrPipeName)
		throw "In CLDataPosterChannelByNamedPipeMaintainer::CLDataPosterChannelByNamedPipeMaintainer(), para error";

	m_pNamedPipe = new CLNamedPipe(pStrPipeName, isSharedPipe, PIPE_FOR_WRITE);
}

// CLDataPosterChannelByNamedPipeMaintainer::CLDataPosterChannelByNamedPipeMaintainer(CLEpollWapper *pEpollHandler, const char* pStrPipeName, bool isSharedPipe)
// {
// 	if(!pEpollHandler || !pStrPipeName)
// 		throw "In CLDataPosterChannelByNamedPipeMaintainer::CLDataPosterChannelByNamedPipeMaintainer(), para error";

// 	m_pEpollHandler = pEpollHandler;
// 	m_pNamedPipe = new CLNamedPipe(pStrPipeName, isSharedPipe, PIPE_FOR_WRITE);
// 	m_pEvent = 0;
// }

CLDataPosterChannelByNamedPipeMaintainer::~CLDataPosterChannelByNamedPipeMaintainer()
{
	if(m_pNamedPipe)
		delete m_pNamedPipe;
}

CLStatus CLDataPosterChannelByNamedPipeMaintainer::Initialize(void *pContext)
{
	// if(m_pEvent == 0)
	// 	return InitializeForEpoll(pContext);
	// else if(m_pEpollHandler == 0)
	// 	return InitializeForEvent(pContext);

	return CLStatus(0, 0);
}

CLStatus CLDataPosterChannelByNamedPipeMaintainer::UnInitialize(void *pContext)
{
	if(m_bIsSharedPipe)
		return CLStatus(0, 0);

	if(!(m_pNamedPipe->OpenRead()).IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataPosterChannelByNamedPipeMaintainer::UnInitialize(), (m_pNamedPipe->OpenRead()) error", 0);
		return CLStatus(-1, 0);
	}

	while(1)
	{
		CLMessage *pMsg;
		pMsg = NULL;

		CLStatus s = m_pNamedPipe->Read((char*)&pMsg, sizeof(CLMessage *));
		if(s.m_clReturnCode <= 0)
		{
			break;
		}
		delete pMsg;
	}

	return CLStatus(0, 0);	
}

CLDataPoster* CLDataPosterChannelByNamedPipeMaintainer::GetDataPoster()
{
	return (new CLDataPosterByNamedPipe(m_pNamedPipe));
}

// CLStatus CLDataPosterChannelByNamedPipeMaintainer::InitializeForEvent()
// {


// 	return CLStatus(0, 0);
// }

// CLStatus CLDataPosterChannelByNamedPipeMaintainer::InitializeForEpoll()
// {

// 	return CLStatus(0, 0);
// }

