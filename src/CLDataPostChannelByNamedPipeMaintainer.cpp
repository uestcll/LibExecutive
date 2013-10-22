#include "CLDataPostChannelByNamedPipeMaintainer.h"
#include "CLInitialDataPostChannelNotifier.h"
#include "CLDataPosterByNamedPipe.h"
#include "ErrorCode.h"

CLDataPostChannelByNamedPipeMaintainer::CLDataPostChannelByNamedPipeMaintainer(const char *pstrNamedPipe) : m_NamedPipe(pstrNamedPipe, false, 0)
{
}

CLDataPostChannelByNamedPipeMaintainer::~CLDataPostChannelByNamedPipeMaintainer()
{
}

CLStatus CLDataPostChannelByNamedPipeMaintainer::Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext)
{
	pNotifier->Notify(DATA_POSTER_INITIALIZE_SUCCESS);
	delete pNotifier;

	return CLStatus(0, 0);
}

CLStatus CLDataPostChannelByNamedPipeMaintainer::Uninitialize(void *pContext)
{
	return CLStatus(0, 0);
}

CLDataPoster* CLDataPostChannelByNamedPipeMaintainer::GetDataPoster()
{
	new CLDataPosterByNamedPipe(&m_NamedPipe);
}