#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "CLLogger.h"
#include "CLNamedPipe.h"

using namespace std;

CLNamedPipe::CLNamedPipe(const char *pstrNamedPipe)
{
	if((pstrNamedPipe == NULL) || (strlen(pstrNamedPipe) == 0))
		throw "In CLNamedPipe::CLNamedPipe(), pstrPipeName error";

	m_strNamedPipe = pstrNamedPipe;

	m_lSizeForAtomWriting = pathconf(m_strNamedPipe.c_str(), _PC_PIPE_BUF);
	if(m_lSizeForAtomWriting == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), pathconf error", errno);
		throw "In CLNamedPipe::CLNamedPipe(), pathconf error";
	}

	if((mkfifo(m_strNamedPipe.c_str(), S_IRUSR | S_IWUSR) == -1) && (errno != EEXIST))
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), mkfifo error", errno);
		throw "In CLNamedPipe::CLNamedPipe(), mkfifo error";
	}

	m_Fd = open(m_strNamedPipe.c_str(), O_RDONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), open error", errno);
		throw "In CLNamedPipe::CLNamedPipe(), open error";
	}
}

CLNamedPipe::~CLNamedPipe()
{
	if(close(m_Fd) == -1)
		CLLogger::WriteLogMsg("In CLNamedPipe::~CLNamedPipe(), close error", errno);
}

int CLNamedPipe::GetFd()
{
	return m_Fd;
}

long CLNamedPipe::GetSizeForAtomWriting()
{
	return m_lSizeForAtomWriting;
}

CLStatus CLNamedPipe::Read(char *pBuf, unsigned long ulBufSize)
{
	long count = read(m_Fd, pBuf, ulBufSize);
	long err = 0;

	if(count == -1)
	{
		err = errno;
		CLLogger::WriteLogMsg("In CLNamedPipe::Read(), read error", errno);
	}

	return CLStatus(count, err);
}

CLStatus CLNamedPipe::Write(char *pBuf, unsigned long ulBufSize)
{
	long count = write(m_Fd, pBuf, ulBufSize);
	long err = 0;

	if(count == -1)
	{
		err = errno;
		CLLogger::WriteLogMsg("In CLNamedPipe::Write(), write error", errno);
	}

	return CLStatus(count, err);
}