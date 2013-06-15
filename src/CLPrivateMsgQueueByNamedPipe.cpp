#include <errno.h>
#include "CLLogger.h"
#include "CLPrivateMsgQueueByNamedPipe.h"

CLPrivateMsgQueuebyNamedPipe::CLPrivateMsgQueuebyNamedPipe(const char *pstrPipeName) : CLMessageQueueByNamedPipe(pstrPipeName)
{
}

CLPrivateMsgQueuebyNamedPipe::~CLPrivateMsgQueuebyNamedPipe()
{
}

CLMessage *CLPrivateMsgQueuebyNamedPipe::ReadMsgFromPipe(int fd)
{
	CLMessage *pMsg = NULL;

	if(read(fd, &pMsg, sizeof(CLMessage *)) == -1)
		CLLogger::WriteLogMsg("In CLPrivateMsgQueuebyNamedPipe::ReadMsgFromPipe(), read error", errno);

	return pMsg;
}