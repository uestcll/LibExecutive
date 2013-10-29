#include "CLDataPosterChannelByNamedPipeMaintainer.h"
#include "CLLogger.h"
#include "CLEpollWapper.h"
#include "CLNamedPipe.h"
#include "CLProtocolDataPoster.h"

CLDataPosterChannelByNamedPipeMaintainer::CLDataPosterChannelByNamedPipeMaintainer(CLEvent *pEvent, const char* pStrPipeName, bool isSharedPipe)
{
	if(!pEvent || !pStrPipeName)
		throw "In CLDataPosterChannelByNamedPipeMaintainer::CLDataPosterChannelByNamedPipeMaintainer(), para error";

	m_pEvent = pEvent;
	m_pNamedPipe = new CLNamedPipe(pStrPipeName, isSharedPipe, PIPE_FOR_WRITE);
	m_pEpollHandler = 0;

}

CLDataPosterChannelByNamedPipeMaintainer::CLDataPosterChannelByNamedPipeMaintainer(CLEpollWapper *pEpollHandler, const char* pStrPipeName, bool isSharedPipe)
{
	if(!pEpollHandler || !pStrPipeName)
		throw "In CLDataPosterChannelByNamedPipeMaintainer::CLDataPosterChannelByNamedPipeMaintainer(), para error";

	m_pEpollHandler = pEpollHandler;
	m_pNamedPipe = new CLNamedPipe(pStrPipeName, isSharedPipe, PIPE_FOR_WRITE);
	m_pEvent = 0;
}

CLDataPosterChannelByNamedPipeMaintainer::~CLDataPosterChannelByNamedPipeMaintainer()
{

}

CLStatus CLDataPosterChannelByNamedPipeMaintainer::Initialize(CLProtocolDataPoster *pProtoDataPoster, void *pContext)
{
	m_pProtoDataPoster = pProtoDataPoster;
	if(m_pEvent == 0)
		return InitializeForEpoll(pContext);
	else if(m_pEpollHandler == 0)
		return InitializeForEvent(pContext);

	return CLStatus(0, 0);
}

CLStatus CLDataPosterChannelByNamedPipeMaintainer::InitializeForEvent()
{


	return CLStatus(0, 0);
}

CLStatus CLDataPosterChannelByNamedPipeMaintainer::InitializeForEpoll()
{

	return CLStatus(0, 0);
}

CLStatus CLDataPosterChannelByNamedPipeMaintainer::UnInitialize(void *pContext)
{


	return CLStatus(0, 0);
}