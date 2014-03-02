#include "CLClientArrivedMsgDeserializer.h"
#include "CLClientArrivedMsg.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"

CLClientArrivedMsgDeserializer::CLClientArrivedMsgDeserializer()
{
}

CLClientArrivedMsgDeserializer::~CLClientArrivedMsgDeserializer()
{
}

CLStatus CLClientArrivedMsgDeserializer::Deserialize(CLIOVectors& IOVectors, CLMessage **ppMsg, CLBufferManager& BufferManager)
{
	try
	{
		if(IOVectors.Size() != sizeof(CLSocket *))
			throw CLStatus(-1, NORMAL_ERROR);

		unsigned long addr = 0;
		CLStatus s = IOVectors.ReadBlock(0, (char *)(&addr), sizeof(unsigned long));
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLClientArrivedMsgDeserializer::Deserialize(), IOVectors.ReadBlock error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		if((s.m_clReturnCode != sizeof(unsigned long)) || (addr == 0))
			throw CLStatus(-1, NORMAL_ERROR);

		*ppMsg = new CLClientArrivedMsg((CLSocket *)addr);

		return CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		*ppMsg = 0;
		return s;
	}
}