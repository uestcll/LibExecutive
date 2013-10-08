#include "CLPointerToMsgDeserializer.h"
#include "CLIOVectors.h"
#include "CLMessage.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"

CLPointerToMsgDeserializer::CLPointerToMsgDeserializer()
{
}

CLPointerToMsgDeserializer::~CLPointerToMsgDeserializer()
{
}

CLStatus CLPointerToMsgDeserializer::Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager)
{
	if(IOVectors.Size() != sizeof(CLMessage *))
	{
		*ppMsg = 0;
		CLLogger::WriteLogMsg("In CLPointerToMsgDeserializer::Deserialize(), IOVectors.Size error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	unsigned long addr = 0;
	CLStatus s = IOVectors.ReadBlock(0, (char *)(&addr), sizeof(unsigned long));
	if((!s.IsSuccess()) || (s.m_clReturnCode != sizeof(unsigned long)) || (addr == 0))
	{
		*ppMsg = 0;
		CLLogger::WriteLogMsg("In CLPointerToMsgDeserializer::Deserialize(), IOVectors.ReadBlock error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	*ppMsg = (CLMessage *)addr;

	return CLStatus(0, 0);
}