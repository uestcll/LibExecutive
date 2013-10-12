#include "CLDataPostChannelBySTLqueueMaintainer.h"
#include "CLDataPosterBySTLqueue.h"
#include "CLSTLqueue.h"
#include "CLInitialDataPostChannelNotifier.h"
#include "ErrorCode.h"

CLDataPostChannelBySTLqueueMaintainer::CLDataPostChannelBySTLqueueMaintainer(CLSTLqueue *pSTLqueue)
{
	if(pSTLqueue == 0)
		throw "In CLDataPostChannelBySTLqueueMaintainer::CLDataPostChannelBySTLqueueMaintainer(), pSTLqueue == 0";

	m_pSTLqueue = pSTLqueue;
}

CLDataPostChannelBySTLqueueMaintainer::~CLDataPostChannelBySTLqueueMaintainer()
{
	delete m_pSTLqueue;
}

CLStatus CLDataPostChannelBySTLqueueMaintainer::Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext)
{
	if(pNotifier)
	{
		pNotifier->Notify(DATA_POSTER_INITIALIZE_SUCCESS);
		delete pNotifier;
	}

	return CLStatus(0, 0);
}

CLStatus CLDataPostChannelBySTLqueueMaintainer::Uninitialize(void *pContext)
{
	return CLStatus(0, 0);
}

CLDataPoster* CLDataPostChannelBySTLqueueMaintainer::GetDataPoster()
{
	new CLDataPosterBySTLqueue(m_pSTLqueue);
}