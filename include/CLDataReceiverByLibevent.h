#ifndef CLDATARECEIVERBYLIBEVENT_H
#define CLDATARECEIVERBYLIBEVENT_H

#include "CLDataReceiver.h"

class CLDataReceiverByLibevent : public CLDataReceiver
{
    public:
    CLDataReceiverByLibevent();
    virtual ~CLDataReceiverByLibevent();

    virtual CLStatus GetData(CLIOVectors& IOVectors, void *pContext);

    private:
    CLDataReceiverByLibevent(const CLDataReceiverByLibevent&);
    CLDataReceiverByLibevent& operator=(const CLDataReceiverByLibevent&);

    private:
};


#endif

