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
	if((Length < sizeof(unsigned long)) || (Length > IOVectorsForData.Size()) || (Length % sizeof(unsigned long) != 0))
	{
		CLLogger::WriteLogMsg("In CLProtocolDecapsulatorBySplitPointer::Decapsulate(), Length error", 0);
		return CLStatus(-1, NORMAL_ERROR);
	}

	CLIteratorForIOVectors iter;
	IOVectorsForData.GetIterator(0, iter);

	while(true)
	{
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