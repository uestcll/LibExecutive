#include "CLMsgToPointerSerializer.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLMsgToPointerSerializer::CLMsgToPointerSerializer()
{
}

CLMsgToPointerSerializer::~CLMsgToPointerSerializer()
{
}

CLStatus CLMsgToPointerSerializer::Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
{
	if((pMsg == 0) || (pIOVectors == 0))
		return CLStatus(-1, NORMAL_ERROR);

	char *p = new char[sizeof(CLMessage *)];
	*((unsigned long *)p) = (unsigned long)pMsg;

	CLStatus s = pIOVectors->PushBack(p, sizeof(CLMessage *), true);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgToPointerSerializer::Serialize(), pIOVectors->PushBack error", 0);
		return s;
	}
	
	return CLStatus(0, 0);
}