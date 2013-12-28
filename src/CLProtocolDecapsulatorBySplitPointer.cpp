#include <vector>
#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLIOVectors.h"
#include "CLIteratorForIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"

using namespace std;

CLProtocolDecapsulatorBySplitPointer::CLProtocolDecapsulatorBySplitPointer() : CLProtocolDecapsulator(0)
{
}

CLProtocolDecapsulatorBySplitPointer::~CLProtocolDecapsulatorBySplitPointer()
{
}

CLStatus CLProtocolDecapsulatorBySplitPointer::Decapsulate(CLIOVectors& IOVectorsForData, unsigned int Length, std::vector<CLIOVectors *>& vSerializedMsgs, CLIOVectors& IOVectorsForPartialData, void *pContext)
{
	if((Length > IOVectorsForData.Size()) || (Length == 0))
		return CLStatus(-1, NORMAL_ERROR);

	CLIteratorForIOVectors iter;
	IOVectorsForData.GetIterator(0, iter);

	unsigned int LeftLength = Length;

	while(true)
	{
		if((LeftLength == 0) || (iter.IsEnd()))
			return CLStatus(0, 0);

		if(LeftLength < sizeof(long))
		{
			if(IOVectorsForData.PushBackRangeToAIOVector(IOVectorsForPartialData, iter, LeftLength).IsSuccess() == false)
			{
				CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), IOVectorsForData.PushBackRangeToAIOVector error", 0);
				return CLStatus(-1, NORMAL_ERROR);
			}

			return CLStatus(0, 0);
		}

		CLIOVectors *pTmp = new CLIOVectors;
		if(IOVectorsForData.PushBackRangeToAIOVector(*pTmp, iter, sizeof(long)).IsSuccess() == false)
		{
			delete pTmp;
			CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), IOVectorsForData.PushBackRangeToAIOVector error 2", 0);
			return CLStatus(-1, NORMAL_ERROR);
		}

		vSerializedMsgs.push_back(pTmp);

		LeftLength = LeftLength - sizeof(long);
	}
}