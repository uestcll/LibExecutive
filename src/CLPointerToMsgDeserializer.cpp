#include "CLPointerToMsgDeserializer.h"
#include "CLIOVectors.h"
#include "CLMessage.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLPointerToMsgDeserializer::CLPointerToMsgDeserializer()
{
}

CLPointerToMsgDeserializer::~CLPointerToMsgDeserializer()
{
}

CLStatus CLPointerToMsgDeserializer::Deserialize(CLIOVectors *pIOVectors, CLMessage **ppMsg)
{
	if(pIOVectors->Size() != sizeof(CLMessage *))
	{
		*ppMsg = 0;
		CLLogger::WriteLogMsg("In CLPointerToMsgDeserializer::Deserialize(), pIOVectors->Size error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	long addr = 0;
	CLStatus s = pIOVectors->ReadLong(0, addr);
	if((!s.IsSuccess()) || (addr == 0))
	{
		*ppMsg = 0;
		CLLogger::WriteLogMsg("In CLPointerToMsgDeserializer::Deserialize(), pIOVectors->ReadLong error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	*ppMsg = (CLMessage *)addr;

	return CLStatus(0, 0);
}