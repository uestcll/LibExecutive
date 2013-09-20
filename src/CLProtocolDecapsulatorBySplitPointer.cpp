#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLProtocolDecapsulatorBySplitPointer::CLProtocolDecapsulatorBySplitPointer() : CLProtocolDecapsulator(0)
{
}

CLProtocolDecapsulatorBySplitPointer::~CLProtocolDecapsulatorBySplitPointer()
{
}

CLStatus CLProtocolDecapsulatorBySplitPointer::Decapsulate(CLIOVectors& IOVectors, std::vector<CLIOVectors *>& vSerializedMsgs, SLMessageScopeInIOVectors *pPartialMsgScope, void *pContext)
{
	pPartialMsgScope->Index = 0;
	pPartialMsgScope->Length = 0;

	size_t length = IOVectors.Size();
	if((length == 0) || (length % sizeof(long) != 0))
	{
		CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), length error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	for(int i = 0; i < length;)
	{
		CLIOVectors *pio = new CLIOVectors;
		CLStatus s = IOVectors.PushBackRangeToAIOVector(*pio, i, sizeof(long));
		if(!s.IsSuccess())
		{
			delete pio;
			CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), IOVectors.PushBackRangeToAIOVector error", 0);
			return CLStatus(-1, NORMAL_ERROR);
		}

		vSerializedMsgs.push_back(pio);

		i += sizeof(long);
	}

	return CLStatus(0, 0);
}