#include <errno.h>
#include "CLDataReceiverByNamedPipe.h"
#include "CLLogger.h"
#include "CLIOVectors.h"
#include "ErrorCode.h"

CLDataReceiverByNamedPipe::CLDataReceiverByNamedPipe(const char *pstrNamedPipe) : m_NamedPipe(pstrNamedPipe, true, 0)
{
}

CLDataReceiverByNamedPipe::~CLDataReceiverByNamedPipe()
{
}

CLStatus CLDataReceiverByNamedPipe::GetData(CLIOVectors& IOVectors, void *pContext)
{
	if(IOVectors.Size() == 0)
		return CLStatus(-1, NORMAL_ERROR);

	*((long *)pContext) = (long)m_NamedPipe.GetFd();

	CLStatus s = m_NamedPipe.Read(IOVectors);
	if(s.m_clReturnCode > 0)
		return s;
	else if(s.m_clReturnCode == 0)
		return CLStatus(-1, RECEIVED_ZERO_BYTE);
	else if(s.m_clErrorCode == EAGAIN)
		return CLStatus(-1, RECEIVED_ZERO_BYTE);
	else
	{
		CLLogger::WriteLogMsg("In CLDataReceiverByNamedPipe::GetData(), m_NamedPipe.Read error", s.m_clErrorCode);
		return CLStatus(-1, NORMAL_ERROR);
	}
}

CLUuid CLDataReceiverByNamedPipe::GetUuid()
{
	return m_NamedPipe.GetUuid();
}

int CLDataReceiverByNamedPipe::GetFd()
{
	return m_NamedPipe.GetFd();
}