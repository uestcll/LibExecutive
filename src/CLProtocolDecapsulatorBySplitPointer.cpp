#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLIOVectors.h"
#include "CLIteratorForIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLProtocolDecapsulatorBySplitPointer::CLProtocolDecapsulatorBySplitPointer() : CLProtocolDecapsulator(0)
{
}

CLProtocolDecapsulatorBySplitPointer::~CLProtocolDecapsulatorBySplitPointer()
{
}

CLStatus CLProtocolDecapsulatorBySplitPointer::Decapsulate(CLIOVectors& IOVectorsForData, std::vector<CLIOVectors& >& vSerializedMsgs, CLIOVectors& IOVectorForPartialData, void *pContext)
{
	size_t length = IOVectorsForData.Size();
	if((length == 0) || (length % sizeof(long) != 0))
	{
		CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), length error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	CLIteratorForIOVectors iter;
	IOVectorsForData.GetIterator(0, iter);

	for(; !iter.IsEnd(); )
	{
		unsigned long data;
		CLStatus s1 = IOVectorsForData.ReadBlock(iter, (char *)&data, sizeof(unsigned long));
		if((!s1.IsSuccess()) || (s1.m_clReturnCode != sizeof(unsigned long)))
		{
			CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), IOVectorsForData.ReadBlock error", 0);
			//......
		}

		CLIOVectors *pio = new CLIOVectors;
		pio//.....................

		vSerializedMsgs.push_back(*pio);
	}

	for(int i = 0; i < length;)
	{
		CLIOVectors *pio = new CLIOVectors;
		CLStatus s = IOVectorsForData.PushBackRangeToAIOVector(*pio, i, sizeof(long));
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