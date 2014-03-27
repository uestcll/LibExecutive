#ifndef CLDATAPOSTCHANNELBYLIBEVENTMAINTAINER_H
#define CLDATAPOSTCHANNELBYLIBEVENTMAINTAINER_H

#include "CLDataPostChannelMaintainer.h"

class CLDataPostChannelByLibeventMaintainer : public CLDataPostChannelMaintainer
{
    public:
    CLDataPostChannelByLibeventMaintainer();
    virtual ~CLDataPostChannelByLibeventMaintainer();

    virtual CLStatus Initialize(CLInitialDataPostChannelNotifier *pNotifier, void *pContext);
    virtual CLStatus Uninitialize(void *pContext);

    virtual CLDataPoster *GetDataPoster();

    private:
    CLDataPostChannelByLibeventMaintainer(const CLDataPostChannelByLibeventMaintainer&);
    CLDataPostChannelByLibeventMaintainer& operator=(const CLDataPostChannelByLibeventMaintainer&);
};

#endif
