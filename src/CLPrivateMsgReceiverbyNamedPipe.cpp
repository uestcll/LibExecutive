#include <errno.h>
#include "CLLogger.h"
#include "CLPrivateMsgReceiverbyNamedPipe.h"
#include <unistd.h>
#include <fcntl.h>

CLPrivateMsgReceiverbyNamedPipe::CLPrivateMsgReceiverbyNamedPipe(const char *pstrPipeName) : CLMessageQueueByNamedPipe(pstrPipeName)
{
}

CLPrivateMsgReceiverbyNamedPipe::~CLPrivateMsgReceiverbyNamedPipe()
{
}

CLStatus CLPrivateMsgReceiverbyNamedPipe::ReadMsgFromPipe(int fd)
{
	while(1)
	{
		CLMessage *pMsg = NULL;
		int ret = read(fd, &pMsg, sizeof(CLMessage *));
		if(ret == -1 || ret == 0)
		{
			CLLogger::WriteLogMsg("In CLPrivateMsgReceiverbyNamedPipe::ReadMsgFromPipe(), read error", errno);
			break;
		}
		else if(ret == sizeof(CLMessage *))
		{
			m_MessageQueue.push(pMsg);
		}
		else
		{
			CLLogger::WriteLogMsg("In CLPrivateMsgReceiverbyNamedPipe::ReadMsgFromPipe(), read length < sizeof(pMsg)", errno);
			break;
		}
	}

	return CLStatus(0, 0);
}
