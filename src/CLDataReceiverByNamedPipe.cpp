#include "CLDataReceiverByNamedPipe.h"
#include "CLLogger.h"
#include "CLIOVectors.h"
#include "ErrorCode.h"

CLDataReceiverByNamedPipe::CLDataReceiverByNamedPipe(const char *pstrNamedPipe) : m_NamedPipe(pstrNamedPipe)
{
}

CLDataReceiverByNamedPipe::~CLDataReceiverByNamedPipe()
{
}

CLStatus CLDataReceiverByNamedPipe::GetData(CLIOVectors& IOVectors, void **ppContext)
{
	(int)(*ppContext) = m_NamedPipe.GetFd();

	CLStatus s = m_NamedPipe.Read(IOVectors);
	if(s.m_clReturnCode > 0)
		return s;
	else if(s.m_clReturnCode == 0)
		return CLStatus(-1, RECEIVED_ZERO_BYTE);
	else
		return CLStatus(-1, NORMAL_ERROR);
}