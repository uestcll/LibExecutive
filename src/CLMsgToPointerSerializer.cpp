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
	{
		CLLogger::WriteLogMsg("In CLMsgToPointerSerializer::Serialize(), parameters error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	char *p = new char[sizeof(CLMessage *)];
	*((unsigned long *)p) = (unsigned long)pMsg;

	pIOVectors->PushBack(p, sizeof(CLMessage *));
	
	return CLStatus(0, 0);
}