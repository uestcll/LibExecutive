#include "CLDataReceiverByNamedPipe.h"
#include "CLNamedPipe.h"
#include "CLBuffer.h"
#include "CLStatus.h"
#include "CLLogger.h"

CLDataReceiverByNamedPipe::CLDataReceiverByNamedPipe(CLNamedPipe *pNamedPipe)
{
	if(pNamedPipe == 0)
		throw "In CLDataReceiverByNamedPipe::CLDataReceiverByNamedPipe(), pNamedPipe is 0";

	m_pNamedPipe = pNamedPipe;
}

CLDataReceiverByNamedPipe::~CLDataReceiverByNamedPipe()
{
	if(m_pNamedPipe != NULL)
	{
		delete m_pNamedPipe;
		m_pNamedPipe = NULL;
	}
}

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
		
		CLStatus s2 = m_pNamedPipe->ReadData(pBuf, restSumLen);
		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLDataReceiverByNamedPipe::GetData(), m_pNamedPipe->ReadData error", 0);
			return s2;
		}
		long len = s2.m_clReturnCode;
		readLen += len;
		if(restSumLen > (int)len || len ==0)
		{
			break;
		}
	}
	
	return CLStatus(readLen, 0);
}