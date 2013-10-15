#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include "CLLogger.h"
#include "CLNamedPipe.h"
#include "ErrorCode.h"
#include "CLIOVectors.h"
#include "CLCriticalSection.h"

using namespace std;

#define MUTEX_FOR_NAMED_PIPE_CLNAMEDPIPE "Mutex_For_Named_Pipe_CLNamedPipe"

CLNamedPipe::CLNamedPipe(const char *pstrNamedPipe) : m_Mutex(MUTEX_FOR_NAMED_PIPE_CLNAMEDPIPE, MUTEX_USE_SHARED_PTHREAD)
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

CLStatus CLNamedPipe::Read(CLIOVectors& IOVectors)
{
	if(IOVectors.Size() <= m_lSizeForAtomWriting)
	{
		return UnsaftyRead(IOVectors);
	}
	else
	{
		CLCriticalSection cs(&m_Mutex);

		return UnsaftyRead(IOVectors);
	}
}

CLStatus CLNamedPipe::Write(CLIOVectors& IOVectors)
{
	if(IOVectors.Size() <= m_lSizeForAtomWriting)
	{
		return UnsaftyWrite(IOVectors);
	}
	else
	{
		CLCriticalSection cs(&m_Mutex);

		return UnsaftyWrite(IOVectors);
	}
}

CLStatus CLNamedPipe::UnsaftyRead(CLIOVectors& IOVectors)
{
	struct iovec *iov = IOVectors.GetIOVecArray();
	if(iov == 0)
		return CLStatus(-1, NORMAL_ERROR);

	long count = readv(m_Fd, iov, IOVectors.GetNumberOfIOVec());
	long err = 0;

	if(count == -1)
	{
		err = errno;
		CLLogger::WriteLogMsg("In CLNamedPipe::Read(), readv error", errno);
	}

	delete [] iov;

	return CLStatus(count, err);
}

CLStatus CLNamedPipe::UnsaftyWrite(CLIOVectors& IOVectors)
{
	struct iovec *iov = IOVectors.GetIOVecArray();
	if(iov == 0)
		return CLStatus(-1, NORMAL_ERROR);

	long count = writev(m_Fd, iov, IOVectors.GetNumberOfIOVec());
	long err = 0;

	if(count == -1)
	{
		err = errno;
		CLLogger::WriteLogMsg("In CLNamedPipe::Write(), writev error", errno);
	}

	delete [] iov;

	return CLStatus(count, err);
}