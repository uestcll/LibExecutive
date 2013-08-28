#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>
#include <string.h>
#include "CLLogger.h"
#include "CLMessageReceiverByNamedPipe.h"

using namespace std;

#define FILE_PATH_FOR_NAMED_PIPE "/tmp/"

CLMessageReceiverByNamedPipe::CLMessageReceiverByNamedPipe(const char *pstrPipeName) 
{
	if((pstrPipeName == NULL) || (strlen(pstrPipeName) == 0))
		throw "In CLMessageReceiverByNamedPipe::CLMessageReceiverByNamedPipe(), pstrPipeName error";

	string strFilePath = FILE_PATH_FOR_NAMED_PIPE;
	strFilePath += pstrPipeName;

	if((mkfifo(strFilePath.c_str(), S_IRUSR | S_IWUSR) == -1) && (errno != EEXIST))
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverByNamedPipe::CLMessageReceiverByNamedPipe(), mkfifo error", errno);
		throw "In CLMessageReceiverByNamedPipe::CLMessageReceiverByNamedPipe(), mkfifo error";
	}

	m_Fd = open(strFilePath.c_str(), O_RDONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverByNamedPipe::CLMessageReceiverByNamedPipe(), open error", errno);
		throw "In CLMessageReceiverByNamedPipe::CLMessageReceiverByNamedPipe(), open error";
	}
}

CLMessageReceiverByNamedPipe::~CLMessageReceiverByNamedPipe()
{
	if(close(m_Fd) == -1)
		CLLogger::WriteLogMsg("In CLMessageReceiverByNamedPipe::~CLMessageReceiverByNamedPipe(), close error", errno);
}

CLMessage* CLMessageReceiverByNamedPipe::GetMessage()
{
	
	CLStatus s = ReadMsgFromPipe(m_Fd);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLMessageReceiverByNamedPipe::GetMessage(), ReadMsgFromPipe error", 0);
		return 0;
	}

	if(m_MessageQueue.empty())
		return 0;

	CLMessage *p = m_MessageQueue.front();
	m_MessageQueue.pop();
	return p;
}