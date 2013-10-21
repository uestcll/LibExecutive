#include "CLProtocolDecapsulatorByDefaultMsgFormat.h"
#include "ErrorCode.h"
#include "CLLogger.h"

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
		IsDataComplete(IOVectorsForData, LeftLength, iter);

		CLIOVectors *pTmp = new CLIOVectors;
		CLStatus s = IOVectorsForData.PushBackRangeToAIOVector(*pTmp, iter, sizeof(unsigned long));
		if(!s.IsSuccess() || s.m_clReturnCode != sizeof(unsigned long))
		{
			delete pTmp;

			CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), PushBackRangeToAIOVector error", 0);

			for(int i = 0; i < vSerializedMsgs.size(); i++)
				delete vSerializedMsgs[i];

			vSerializedMsgs.clear();

			return CLStatus(-1, NORMAL_ERROR);
		}

		vSerializedMsgs.push_back(pTmp);

		Length = Length - sizeof(unsigned long);

		if(Length < sizeof(unsigned long))
			return CLStatus(0, 0);
	}
}