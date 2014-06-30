#ifndef CLMSGRECEIVERRELEASERFORPRIVATEPIPE_H
#define CLMSGRECEIVERRELEASERFORPRIVATEPIPE_H

#include "CLMsgReceiverReleaser.h"

class CLMessageReceiver;

class CLMsgReceiverReleaserForPrivatePipe : public CLMsgReceiverReleaser
{
public:
	CLMsgReceiverReleaserForPrivatePipe();
	virtual ~CLMsgReceiverReleaserForPrivatePipe();

public:
	virtual CLStatus ReleaseMsgReceiver(CLMessageReceiver *pMsgReceiver);

private:
	CLMsgReceiverReleaserForPrivatePipe(const CLMsgReceiverReleaserForPrivatePipe&);
	CLMsgReceiverReleaserForPrivatePipe& operator=(const CLMsgReceiverReleaserForPrivatePipe&);
};

#endif