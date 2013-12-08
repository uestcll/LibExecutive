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
	
	int len = sizeof(unsigned long) + sizeof(int);
	char *pBuffer = new char[len];

	unsigned long *pID = (unsigned long *)(pBuffer);
	*pID = pM->m_clMsgID;
	int *pEchoID = (int*)(pBuffer + sizeof(unsigned long));
	*pEchoID = pM->m_iEchoID;

	return (pDataVec->PushBack(pBuffer, len));
}