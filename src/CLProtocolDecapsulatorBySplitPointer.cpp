#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLIOVectors.h"
#include "CLLogger.h"

CLProtocolDecapsulatorBySplitPointer::CLProtocolDecapsulatorBySplitPointer() : CLProtocolDecapsulator(0)
{
}

CLProtocolDecapsulatorBySplitPointer::~CLProtocolDecapsulatorBySplitPointer()
{
}

CLStatus CLProtocolDecapsulatorBySplitPointer::Decapsulate(CLIOVectors& IOVectors, std::vector<CLIOVectors>& vSerializedMsgs, SLMessageScopeInIOVectors *pPartialMsgScope, void *pContext)
{
	pPartialMsgScope->Index = 0;
	pPartialMsgScope->Length = 0;

	size_t length = IOVectors.Size();
	if((length == 0) || (length % sizeof(long) != 0))
	{
		CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), length error", 0);
		return CLStatus(-1, 0);
	}

	for(int i = 0; i < length;)
	{
		CLIOVectors tmp_io;
		CLStatus s = IOVectors.PushBackRangeToAIOVector(tmp_io, i, sizeof(long));
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), IOVectors.PushBackRangeToAIOVector error", 0);
			return CLStatus(-1, 0);
		}

		//std::vector<CLIOVectors> is error, change to std::vector<CLIOVectors *>................
		vSerializedMsgs.push_back(tmp_io);

		i += sizeof(long);
	}

	return CLStatus(0, 0);
}