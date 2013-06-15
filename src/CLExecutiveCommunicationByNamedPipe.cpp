#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "CLExecutiveCommunicationByNamedPipe.h"
#include "CLLogger.h"
#include "CLMessage.h"

#define FILE_PATH_FOR_COMMUNICATION_NAMED_PIPE "/tmp/"

CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName, bool bDeleteMsg) : m_Event(pstrExecutiveName, true)
{
	if((pstrExecutiveName == 0) || (strlen(pstrExecutiveName) == 0))
		throw "In CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe(), pstrExecutiveName error";

	m_strExecutiveName = FILE_PATH_FOR_COMMUNICATION_NAMED_PIPE;
	m_strExecutiveName += pstrExecutiveName;

	m_lPipeBufSize = pathconf(m_strExecutiveName.c_str(), _PC_PIPE_BUF);
	if(m_lPipeBufSize == -1)
	{
		CLLogger::WriteLogMsg("In CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe(), pathconf error", errno);
		throw "In CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe(), pathconf error";
	}

	m_Fd = open(m_strExecutiveName.c_str(), O_WRONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMsg("In CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe(), open error", errno);
		throw "In CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe(), open error";
	}

	m_bDeleteMsg = bDeleteMsg;
}

CLExecutiveCommunicationByNamedPipe::~CLExecutiveCommunicationByNamedPipe()
{
	if(close(m_Fd) == -1)
		CLLogger::WriteLogMsg("CLExecutiveCommunicationByNamedPipe::~CLExecutiveCommunicationByNamedPipe(), close", errno);
}

CLStatus CLExecutiveCommunicationByNamedPipe::PostExecutiveMessage(CLMessage *pMessage)
{
	char *pBuf = NULL;

	try
	{
		if(pMessage == NULL)
			return CLStatus(-1, 0);

		unsigned int length;
		pBuf = GetMsgBuf(pMessage, &length);
		if(pBuf == 0)
			throw CLStatus(-1, 0);

		if(length > m_lPipeBufSize)
			throw CLStatus(-1, 0);

		if(write(m_Fd, pBuf, length) == -1)
		{
			CLLogger::WriteLogMsg("In CLExecutiveCommunicationByNamedPipe::PostExecutiveMessage(), write error", errno);
			throw CLStatus(-1, errno);
		}

		if(!m_Event.Set().IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLExecutiveCommunicationByNamedPipe::PostExecutiveMessage(), m_Event.Set() error", 0);
			throw CLStatus(-1, 0);
		}

		throw CLStatus(0, 0);
	}
	catch(CLStatus& s)
	{
		if(!s.IsSuccess())
			delete pMessage;
		else
		{
			if(m_bDeleteMsg)
				delete pMessage;
		}

		if(pBuf != NULL)
			delete [] pBuf;

		return s;
	}
}