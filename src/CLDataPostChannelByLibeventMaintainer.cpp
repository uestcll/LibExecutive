#include "CLDataPostChannelByLibeventMaintainer.h"
#include "CLInitialDataPostChannelNotifier.h"
#include "ErrorCode.h"

CLDataPostChannelByLibeventMaintainer::CLDataPostChannelByLibeventMaintainer
{}

CLDataPostChannelByLibeventMaintainer::~CLDataPostChannelByLibeventMaintainer()
{}

CLStatus CLDataPostChannelByLibeventMaintainer::Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext)
{
    pNotifier->Notify(DATA_POSTER_INITIALIZE_SUCCESS);
    delete pNotifier;

    return CLStatus(0, 0);
}

CLStatus CLDataPostChannelByLibeventMaintainer::Uninitialize(void *pContext)
{
    return CLStatus(0, 0);
}

CLDataPoster* CLDataPostChannelByLibeventMaintainer::GetDataPoster()
{
    
}
