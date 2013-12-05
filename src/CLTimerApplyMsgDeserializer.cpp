#include "CLTimerApplyMsgDeserializer.h"
#include "CLTimerApplyMsg.h"
#include "CLIOVector.h"
#include "CLMessage.h"

CLTimerApplyMsgDeserializer::CLTimerApplyMsgDeserializer()
{

}

CLTimerApplyMsgDeserializer::~CLTimerApplyMsgDeserializer()
{

}

CLStatus CLTimerApplyMsgDeserializer::Deserialize(CLIOVector &dataVec, CLMessage **pMsg)
{
	CLTimerApplyMsg *pTimerApplyMsg = new CLTimerApplyMsg();

	struc itimerspec *pTimeValue = &(pTimerApplyMsg->m_sTimeValue);
	int *pID = &(pTimerApplyMsg->m_iEchoID);

	int nameLen = dataVec.Length() - sizeof(long) - sizeof(struct itimerspec) - sizeof(int)
	char *pName = new char [nameLen];

	dataVec->ReadData((char*)(pTimeValue), sizeof(long), sizeof(struct itimerspec));
	dataVec->ReadData((char*)(pID), sizeof(long) + sizeof(struct itimerspec), sizeof(int));
	dataVec->ReadData(pName, sizeof(long) + sizeof(struct itimerspec) + sizeof(int), nameLen);

	pTimerApplyMsg->m_strRemoteName = pName;

	delete [] pName;
	*pMsg = pTimerApplyMsg;

	return CLStatus(0, 0);
}
