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
#include "DefinitionForConst.h"

using namespace std;

CLNamedPipe::CLNamedPipe(const char* pstrNamedPipe, bool bReader, const char *pstrMutexName)
{
	if((pstrNamedPipe == NULL) || (strlen(pstrNamedPipe) == 0))
		throw "In CLNamedPipe::CLNamedPipe(), pstrNamedPipe error";

	m_strNamedPipe = pstrNamedPipe;

	m_lSizeForAtomWriting = pathconf(m_strNamedPipe.c_str(), _PC_PIPE_BUF);
	if(m_lSizeForAtomWriting == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), pathconf error", errno);
		throw "In CLNamedPipe::CLNamedPipe(), pathconf error";
	}

	m_bReader = bReader;

	if(m_bReader)
	{
		if(!InitialReader().IsSuccess())
			throw "In CLNamedPipe::CLNamedPipe(), InitialReader error";
	}
	else
	{
		if(!InitialWriter().IsSuccess())
			throw "In CLNamedPipe::CLNamedPipe(), InitialWriter error";
	}
	
	if((pstrMutexName != 0) && (strlen(pstrMutexName) != 0))
		m_pMutex = new CLMutex(pstrMutexName, MUTEX_USE_SHARED_PTHREAD);
	else
		m_pMutex = 0;
}

CLStatus CLNamedPipe::InitialReader()
{
	if((mkfifo(m_strNamedPipe.c_str(), S_IRUSR | S_IWUSR) == -1) && (errno != EEXIST))
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::InitialReader(), mkfifo error", errno);
		return CLStatus(-1, NORMAL_ERROR);
	}

	m_Fd = open(m_strNamedPipe.c_str(), O_RDONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::InitialReader(), open error", errno);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return CLStatus(0, 0);
}

CLStatus CLNamedPipe::InitialWriter()
{
	m_Fd = open(m_strNamedPipe.c_str(), O_WRONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::InitialWriter(), open error", errno);
		return CLStatus(-1, NORMAL_ERROR);
	}

	return CLStatus(0, 0);
}

CLNamedPipe::~CLNamedPipe()
{
	if(close(m_Fd) == -1)
		CLLogger::WriteLogMsg("In CLNamedPipe::~CLNamedPipe(), close error", errno);

	if(m_pMutex)
		delete m_pMutex;
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
	if(!m_bReader || (IOVectors.Size() == 0))
		return CLStatus(-1, NORMAL_ERROR);

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
	if(m_bReader || (IOVectors.Size() == 0))
		return CLStatus(-1, NORMAL_ERROR);

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