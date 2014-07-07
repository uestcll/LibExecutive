#include <queue>
#include "CLMsgReceiverReleaserForPrivatePipe.h"
#include "CLMessageLoopManager.h"
#include "ErrorCode.h"
#include "DefinitionForConst.h"
#include "CLMessageReceiver.h"
#include "CLMessage.h"

CLMsgReceiverReleaserForPrivatePipe::CLMsgReceiverReleaserForPrivatePipe()
{
	
}

CLMsgReceiverReleaserForPrivatePipe::~CLMsgReceiverReleaserForPrivatePipe()
{

}

CLStatus CLMsgReceiverReleaserForPrivatePipe::ReleaseMsgReceiver(CLMessageReceiver *pMsgReceiver)
{
	return CLStatus(0, 0);
}