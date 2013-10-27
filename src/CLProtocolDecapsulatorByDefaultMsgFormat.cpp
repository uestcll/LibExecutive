#include <vector>
#include "CLProtocolDecapsulatorByDefaultMsgFormat.h"
#include "CLIOVectors.h"
#include "CLIteratorForIOVectors.h"
#include "CLBufferManager.h"
#include "ErrorCode.h"
#include "CLLogger.h"

using namespace std;

CLProtocolDecapsulatorByDefaultMsgFormat::CLProtocolDecapsulatorByDefaultMsgFormat() : CLProtocolDecapsulator(0)
{
}

CLProtocolDecapsulatorByDefaultMsgFormat::~CLProtocolDecapsulatorByDefaultMsgFormat()
{
}

int CLProtocolDecapsulatorByDefaultMsgFormat::IsDataComplete(CLIOVectors& IOVectorsForData, unsigned int LeftLength, CLIteratorForIOVectors& iter)
{
	unsigned int HeadLength = sizeof(int);
	
	if(LeftLength < HeadLength)
		return -1;

	unsigned int load_len = 0;
	CLStatus s = IOVectorsForData.ReadBlock(iter, (char *)(&load_len), sizeof(unsigned int));
	if(!s.IsSuccess())
		return -2;

	unsigned int total = load_len + HeadLength;
	if(total <= LeftLength)
		return total;
	else
		return -1;
}

CLStatus CLProtocolDecapsulatorByDefaultMsgFormat::Decapsulate(CLIOVectors& IOVectorsForData, unsigned int Length, std::vector<CLIOVectors *>& vSerializedMsgs, CLBufferManager& BufferManager, void *pContext)
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

		int r = IsDataComplete(IOVectorsForData, LeftLength, iter);
		if(r == -2)
			return CLStatus(-1, NORMAL_ERROR);

		if(r == -1)
		{
			CLIOVectors partial;
			CLStatus s = IOVectorsForData.PushBackRangeToAIOVector(partial, iter, IOVectorsForData.Size());
			if(!s.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLProtocolDecapsulatorByDefaultMsgFormat::Decapsulate(), IOVectorsForData.PushBackRangeToAIOVector error", 0);
				return CLStatus(-1, NORMAL_ERROR);
			}

			BufferManager.SetPartialDataIOVector(partial);
			return CLStatus(0, 0);
		}

		CLIOVectors *ptmp = new CLIOVectors;
		CLStatus s1 = IOVectorsForData.PushBackRangeToAIOVector(*ptmp, iter, r);
		if(!s1.IsSuccess())
		{
			delete ptmp;
			CLLogger::WriteLogMsg("In CLProtocolDecapsulatorByDefaultMsgFormat::Decapsulate(), IOVectorsForData.PushBackRangeToAIOVector 2 error", 0);
			return CLStatus(-1, NORMAL_ERROR);
		}

		vSerializedMsgs.push_back(ptmp);

		iter.Add(r);
		LeftLength = LeftLength - r;
	}
}