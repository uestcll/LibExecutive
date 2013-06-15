#include <errno.h>
#include <fcntl.h>
#include <CLMessage.h>
#include "CLLogger.h"
#include "CLPrivateExecutiveCommunicationByNamedPipe.h"

CLPrivateExecutiveCommunicationByNamedPipe::CLPrivateExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName) : CLExecutiveCommunicationByNamedPipe(pstrExecutiveName, false)
{
}

CLPrivateExecutiveCommunicationByNamedPipe::~CLPrivateExecutiveCommunicationByNamedPipe()
{
	int fd = open(m_strExecutiveName.c_str(), O_RDONLY | O_NONBLOCK);
	if(fd == -1) 
	{   
		CLLogger::WriteLogMsg("In CLPrivateExecutiveCommunicationByNamedPipe::~CLPrivateExecutiveCommunicationByNamedPipe(), open error", errno); 
		return;
	}

	for(;;)
	{
		CLMessage *pMsg;
		pMsg = NULL;

		if(read(fd, &pMsg, sizeof(CLMessage *)) == -1) 
			break;

		delete pMsg;
	}   

	if(close(fd) == -1)
		CLLogger::WriteLogMsg("In CLPrivateExecutiveCommunicationByNamedPipe::~CLPrivateExecutiveCommunicationByNamedPipe(), close error", errno); 
}

char *CLPrivateExecutiveCommunicationByNamedPipe::GetMsgBuf(CLMessage *pMsg, unsigned int *pLength)
{
	int len = sizeof(CLMessage *);
	char *pBuf = new char[len];
	*((long *)pBuf) = (long)pMsg;

	*pLength = len;
	return pBuf;
}