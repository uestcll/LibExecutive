#include "CLDataReceiverByNamedPipe.h"
#include "CLBuffer.h"
#include "CLStatus.h"
#include "CLLogger.h"
#include "CLIOVector.h"

CLDataReceiverByNamedPipe::CLDataReceiverByNamedPipe(const char *pStrPipeName, bool isSharedPipe) : m_NamedPipe(pStrPipeName, isSharedPipe, PIPE_FOR_READ)
{
	m_lMaxSizeForPipe = m_NamedPipe.GetAtomWriteSize();
}

CLDataReceiverByNamedPipe::~CLDataReceiverByNamedPipe()
{

}
// no break!!!
CLStatus CLDataReceiverByNamedPipe::GetData(CLIOVector &IOVec)
{
	
	long readLen = 0;

	// CLIOVector IOVec;

	// CLStatus s1 = pBuffer->GetRestIOVecs(IOVec);
	// if(!s1.IsSuccess())
	// {
	// 	CLLogger::WriteLogMsg("In CLDataReceiverByNamedPipe::GetData(), pBuffer->GetRestIOVecs error", 0);
	// 	return s1;
	// }

	CLStatus s2 = m_NamedPipe.ReadVecs(IOVec);
	if(!s2.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLDataReceiverByNamedPipe::GetData(), m_NamedPipe->ReadData error", 0);
		return s2;
	}
	long len = s2.m_clReturnCode;
	readLen += len;


	/*
	char *pBuf = 0;
	int restSumLen = 0;
	while(1)
	{
		CLStatus s1 = pBuffer->GetRestBufPtr(&pBuf, restSumLen);

		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLDataReceiverByNamedPipe::GetData(), pBuffer->GetRestBufPtr error", 0);
			return s1;
		}		
		
		CLStatus s2 = m_NamedPipe.Read(pBuf, restSumLen);
		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLDataReceiverByNamedPipe::GetData(), m_NamedPipe->ReadData error", 0);
			return s2;
		}
		long len = s2.m_clReturnCode;
		readLen += len;
		if(restSumLen > len || len == 0 || readLen >= m_lMaxSizeForPipe)
		{
			break;
		}
	}
	*/
	return CLStatus(readLen, 0);
}

const int CLDataReceiverByNamedPipe::GetFd()
{
	return m_NamedPipe.GetPipeFd();
}