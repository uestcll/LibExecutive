#include <vector>
#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLIOVectors.h"
#include "CLIteratorForIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"

using namespace std;

CLProtocolDecapsulatorBySplitPointer::CLProtocolDecapsulatorBySplitPointer() : CLProtocolDecapsulator(0)
{
}

CLProtocolDecapsulatorBySplitPointer::~CLProtocolDecapsulatorBySplitPointer()
{
}

CLStatus CLProtocolDecapsulatorBySplitPointer::Decapsulate(CLIOVectors& IOVectorsForData, unsigned int Length, std::vector<CLIOVectors *>& vSerializedMsgs, CLBufferManager& BufferManager, void *pContext)
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
			CLIOVectors partial;
			IOVectorsForData.PushBackRangeToAIOVector(partial, iter, LeftLength);
			BufferManager.SetPartialDataIOVector(partial);
			return CLStatus(0, 0);
		}

		CLIOVectors *pTmp = new CLIOVectors;
		IOVectorsForData.PushBackRangeToAIOVector(*pTmp, iter, sizeof(long));
		vSerializedMsgs.push_back(pTmp);

		LeftLength = LeftLength - sizeof(long);
	}
}