#include "CLTimerOutNotifyMsgDeserializer.h"
#include "CLIOVector.h"
#include "CLTimerOutNotifyMsg.h"
#include "CLMessage.h"


CLTimerOutNotifyMsgDeserializer::CLTimerOutNotifyMsgDeserializer()
{

}

CLTimerOutNotifyMsgDeserializer::~CLTimerOutNotifyMsgDeserializer()
{

}

CLStatus CLTimerOutNotifyMsgDeserializer::Deserialize(CLIOVector &dataVec, CLMessage **pMsg)
{
	CLTimerOutNotifyMsg *pTimerOutMsg = new CLTimerOutNotifyMsg();

	int *pID = &(pTimerOutMsg->m_iEchoID);
	dataVec->ReadData((char*)pID, sizeof(long), sizeof(int));

	*pMsg = pTimerOutMsg;

	return CLStatus(0, 0);
}
