#ifndef CLMSGRECEIVERRELEASER_H
#define CLMSGRECEIVERRELEASER_H

#include "CLStatus.h"

class CLMessageReceiver;

class CLMsgReceiverReleaser
{
public:
	CLMsgReceiverReleaser();
	virtual ~CLMsgReceiverReleaser();

public:
	virtual CLStatus ReleaseMsgReceiver(CLMessageReceiver *pMsgReceiver) = 0;

private:
	CLMsgReceiverReleaser(const CLMsgReceiverReleaser&);
	CLMsgReceiverReleaser& operator=(const CLMsgReceiverReleaser&);
};

#endif