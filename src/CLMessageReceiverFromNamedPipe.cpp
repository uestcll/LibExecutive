#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include <string.h>
#include "CLLogger.h"
#include "CLMessageQueueByNamedPipe.h"

using namespace std;

#define FILE_PATH_FOR_NAMED_PIPE "/tmp/"

CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe(const char *pstrPipeName) : m_Event(pstrPipeName, true)
{
	if((pstrPipeName == NULL) || (strlen(pstrPipeName) == 0))
		throw "In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe(), pstrPipeName error";

	string strFilePath = FILE_PATH_FOR_NAMED_PIPE;
	strFilePath += pstrPipeName;

	if((mkfifo(strFilePath.c_str(), S_IRUSR | S_IWUSR) == -1) && (errno != EEXIST))
	{
		CLLogger::WriteLogMsg("In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe(), mkfifo error", errno);
		throw "In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe(), mkfifo error";
	}

	m_Fd = open(strFilePath.c_str(), O_RDONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMsg("In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe(), open error", errno);
		throw "In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe(), open error";
	}
}

CLMessageQueueByNamedPipe::~CLMessageQueueByNamedPipe()
{
	if(close(m_Fd) == -1)
		CLLogger::WriteLogMsg("In CLMessageQueueByNamedPipe::~CLMessageQueueByNamedPipe(), close error", errno);
}

CLMessage* CLMessageQueueByNamedPipe::GetMessage()
{
	CLStatus s = m_Event.Wait();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageQueueByNamedPipe::GetMessage(), m_Event.Wait error", 0);
		return 0;
	}

	return ReadMsgFromPipe(m_Fd);
}