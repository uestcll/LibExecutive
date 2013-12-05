#include "CLTimerOutNotifyMsgSerializer.h"
#include "CLIOVector.h"
#include "CLTimerOutNotifyMsg.h"
#include "CLMessage.h"

CLTimerOutNotifyMsgSerializer::CLTimerOutNotifyMsgSerializer()
{

}

CLTimerOutNotifyMsgSerializer::~CLTimerOutNotifyMsgSerializer()
{

}

CLStatus CLTimerOutNotifyMsgSerializer::Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
{
	CLTimerOutNotifyMsg *pM = dynamic_cast<CLTimerOutNotifyMsg*>(pMsg);
	if(!pM)
		return CLStatus(-1, 0);
	
	int len = sizeof(int);
	char *pBuffer = new char[len];
	int *pID = (int*)pBuffer;
	*pID = pM->m_iEchoID;

	return (pDataVec->PushBack(pBuffer, len));
}