#include <string.h>
#include <stdio.h>
#include "CLTimerApplyMsgSerializer.h"
#include "CLIOVector.h"
#include "CLTimerApplyMsg.h"

CLTimerApplyMsgSerializer::CLTimerApplyMsgSerializer()
{

}

CLTimerApplyMsgSerializer::~CLTimerApplyMsgSerializer()
{

}

CLStatus CLTimerApplyMsgSerializer::Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
{
	CLTimerApplyMsg *p = dynamic_cast<CLTimerApplyMsg*>(pMsg);
	if(NULL == p)
		return CLStatus(-1, 0);

	int length = sizeof(struct itimerspec) + sizeof(int) + (p->m_strRemoteName.length());
	char *pBuf = new char[length];
	memcpy(pBuf, &(p->m_sTimeValue), sizeof(struct itimerspec));
	
	int *pi = (int *)(pBuf + sizeof(struct itimerspec));
	*pi = p->m_iEchoID;

	memcpy(pBuf + sizeof(struct itimerspec) + sizeof(int), (p->m_strRemoteName).c_str(), (p->m_strRemoteName.length()));

	return (pDataVec->PushBack(pBuf, length));
}