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
	std::queue<SLMessageAndSource*> MessageContainer;

	for(;;)
	{
		CLStatus s1 = pMsgReceiver->GetMessage(MessageContainer);
		if(!s1.IsSuccess())
		{
			if((s1.m_clErrorCode == NORMAL_ERROR) || (s1.m_clErrorCode == MSG_RECEIVED_ZERO))
				break;
		}
	}

	while(!MessageContainer.empty())
	{
		SLMessageAndSource *pInfo = MessageContainer.front();
		MessageContainer.pop();
		if(pInfo && pInfo->pMsg)
		{
			delete pInfo->pMsg;

			delete pInfo;
		}
		else if(pInfo)
		{
			delete pInfo;
		}
	}

	delete pMsgReceiver;

	return CLStatus(0, 0);
}