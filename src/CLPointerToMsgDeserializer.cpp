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

CLStatus CLPointerToMsgDeserializer::Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager, void *pContext)
{
	try
	{
		if(IOVectors.Size() != sizeof(CLMessage *))
			throw CLStatus(-1, NORMAL_ERROR);

		unsigned long addr = 0;
		CLStatus s = IOVectors.ReadBlock(0, (char *)(&addr), sizeof(unsigned long));
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLPointerToMsgDeserializer::Deserialize(), IOVectors.ReadBlock error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		if((s.m_clReturnCode != sizeof(unsigned long)) || (addr == 0))
			throw CLStatus(-1, NORMAL_ERROR);

		*ppMsg = (CLMessage *)addr;

		return CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		*ppMsg = 0;
		return s;
	}
}