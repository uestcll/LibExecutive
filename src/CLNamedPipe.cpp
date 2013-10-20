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
#include "CLMutex.h"

using namespace std;

CLNamedPipe::CLNamedPipe(const char *pstrNamedPipe)
{
	if(InitialNamedPipe(pstrNamedPipe).IsSuccess())
		m_pMutex = 0;
	else
		throw "In CLNamedPipe::CLNamedPipe(), InitialNamedPipe error";
}

CLNamedPipe::CLNamedPipe(const char* pstrNamedPipe, const char *pstrMutexName)
{
	if(InitialNamedPipe(pstrNamedPipe).IsSuccess())
		m_pMutex = new CLMutex(pstrMutexName, MUTEX_USE_SHARED_PTHREAD);
	else
		throw "In CLNamedPipe::CLNamedPipe(), InitialNamedPipe error";
}

CLStatus CLNamedPipe::InitialNamedPipe(const char *pstrNamedPipe)
{
	if((pstrNamedPipe == NULL) || (strlen(pstrNamedPipe) == 0))
		return CLStatus(-1, NORMAL_ERROR);

	m_strNamedPipe = pstrNamedPipe;

	m_lSizeForAtomWriting = pathconf(m_strNamedPipe.c_str(), _PC_PIPE_BUF);
	if(m_lSizeForAtomWriting == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), pathconf error", errno);
		return CLStatus(-1, NORMAL_ERROR);
	}

	if((mkfifo(m_strNamedPipe.c_str(), S_IRUSR | S_IWUSR) == -1) && (errno != EEXIST))
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), mkfifo error", errno);
		return CLStatus(-1, NORMAL_ERROR);
	}

	m_Fd = open(m_strNamedPipe.c_str(), O_RDONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), open error", errno);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return CLStatus(0, 0);
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
	if(m_pMutex)
	{
		CLCriticalSection cs(m_pMutex);

		return UnsaftyRead(IOVectors);
	}
	else
	{
		return UnsaftyRead(IOVectors);
	}
}

CLStatus CLNamedPipe::Write(CLIOVectors& IOVectors)
{
	if(m_pMutex)
	{
		CLCriticalSection cs(m_pMutex);

		return UnsaftyWrite(IOVectors);
	}
	else
	{
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