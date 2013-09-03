#include "CLMsgToPointerSerializer.h"
#include "CLLogger.h"
#include "CLIOVectors.h"

CLMsgToPointerSerializer::CLMsgToPointerSerializer()
{
}

CLMsgToPointerSerializer::~CLMsgToPointerSerializer()
{
}

CLStatus CLMsgToPointerSerializer::Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors)
{
	if((pMsg == 0) || (pIOVectors == 0))
		return CLStatus(-1, 0);

	char *p = new char[sizeof(CLMessage *)];
	*((long *)p) = (long)pMsg;

	CLStatus s = pIOVectors->PushBack(p, sizeof(CLMessage *));
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMsgToPointerSerializer::Serialize(), pIOVectors->PushBack error", 0);
		return s;
	}

	return CLStatus(0, 0);
}