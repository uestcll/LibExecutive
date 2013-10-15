#include "CLDataReceiverByNamedPipe.h"
#include "CLBuffer.h"
#include "CLStatus.h"
#include "CLLogger.h"

CLDataReceiverByNamedPipe::CLDataReceiverByNamedPipe(const char *pStrPipeName) : m_NamedPipe(pStrPipeName, PIPE_FOR_READ)
{
	m_lMaxSize = m_NamedPipe.GetAtomWriteSize();
}

CLDataReceiverByNamedPipe::~CLDataReceiverByNamedPipe()
{

}
// no break!!!
CLStatus CLDataReceiverByNamedPipe::GetData(CLBuffer *pBuffer)
{
	char *pBuf = 0;
	int restSumLen = 0;
	long readLen = 0;


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
		if(restSumLen > len || len ==0 || readLen >= m_lMaxSize)
		{
			break;
		}
	}
	
	return CLStatus(readLen, 0);
}