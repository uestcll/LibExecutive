#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "CLLogger.h"
#include "CLNamedPipe.h"
#include "CLStatus.h"

#define PIPE_FOR_READ 20
#define PIPE_FOR_WRITE 30
#define FILE_PATH_FOR_NAMED_PIPE "/tmp/"

CLNamedPipe::CLNamedPipe(const char* pStrPipeName, int flag)
{
	if(pStrPipeName == NULL || strlen(pStrPipeName) == 0)
		throw "In CLNamedPipe::CLNamedPipe(), pStrPipeName is NULL or len is 0";

	m_strPipeName = FILE_PATH_FOR_NAMED_PIPE;
	m_strPipeName += pStrPipeName;

	if((mkfifo(m_strPipeName.c_str(), S_IRUSR | S_IWUSR) == -1) && (errno != EEXIST))
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), mkfifo error", errno);
		throw "In CLNamedPipe::CLNamedPipe(), mkfifo error";
	}

	if(flag == PIPE_FOR_READ)
	{
		m_Fd = open(m_strPipeName.c_str(), O_RDONLY | O_NONBLOCK);
		if(m_Fd == -1)
		{
			CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), for read open error", errno);
			throw "In CLNamedPipe::CLNamedPipe(), for read open error";
		}
	}
	else if(flag == PIPE_FOR_WRITE)
	{
		m_Fd = open(m_strPipeName.c_str(), O_WRONLY | O_NONBLOCK);
		if(m_Fd == -1)
		{
			CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), for write open error", errno);
			throw "In CLNamedPipe::CLNamedPipe(), for write open error";
		}
	}
	else
	{
		throw "In CLNamedPipe::CLNamedPipe(), flag is error";
	}
}	

CLNamedPipe::~CLNamedPipe()
{

}

CLStatus CLNamedPipe::Read(char *pBuf, int length)
{
	int  readLen = 0;

	readLen = read(m_Fd, pBuf, length);

	if(readLen == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::Read(), read error", errno);
		return CLStatus(-1, errno);
	}

	return CLStatus(readLen, 0)
}

CLStatus CLNamedPipe::Write(char *pBuf, int length)
{
	int writeLen = 0;

	writeLen = write(m_Fd, pBuf, length);

	if(writeLen == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::Write(), write error", errno);
		return CLStatus(-1, errno);
	}

	return CLStatus(writeLen, 0);	
}