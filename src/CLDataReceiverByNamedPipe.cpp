#include "CLDataReceiverByNamedPipe.h"
#include "CLLogger.h"
#include "CLIOVectors.h"

#define PIPE_READ_BUFFER_LENGTH 4096

CLDataReceiverByNamedPipe::CLDataReceiverByNamedPipe(const char *pstrNamedPipe) : m_NamedPipe(pstrNamedPipe)
{
}

CLDataReceiverByNamedPipe::~CLDataReceiverByNamedPipe()
{
}

CLStatus CLDataReceiverByNamedPipe::GetData(CLIOVectors *pIOVectors, void **ppContext)
{
	char *pBuffer = new char[PIPE_READ_BUFFER_LENGTH];
	CLStatus s = m_NamedPipe->Read(pBuffer, PIPE_READ_BUFFER_LENGTH);
	if(s.IsSuccess())
	{
		CLStatus s1 = pIOVectors->PushBack(pBuffer, s.m_clReturnCode);
		if(s1.IsSuccess())
		{
			return CLStatus(0, 0);
		}
	}

	delete [] pBuffer;

	return CLStatus(-1, RECEIVED_ERROR);
}