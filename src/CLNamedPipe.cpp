#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include "CLLogger.h"
#include "CLNamedPipe.h"
#include "CLStatus.h"
#include "CLIOVector.h"
#include "CLCriticalSection.h"

CLNamedPipe::CLNamedPipe(const char* pStrPipeName, bool isShared, int flag)
{
	try
	{
		CLStatus s = Initialize(pStrPipeName, flag);
		if(!s.IsSuccess())
		{
			throw s;
		}
		if(isShared)
			m_pMutexForNamedPipe = new CLMutex(pStrPipeName, MUTEX_USE_SHARED_PTHREAD);
		else
			m_pMutexForNamedPipe = new CLMutex();
	}
	catch(CLStatus& s)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::CLNamedPipe(), Initialize() error", 0);
	}
}

CLNamedPipe::~CLNamedPipe()
{
	if(m_pMutexForNamedPipe != 0)
	{
		delete m_pMutexForNamedPipe;
		m_pMutexForNamedPipe = 0;
	}	
}

const int CLNamedPipe::GetPipeFd() const
{
	return m_Fd;
}

CLStatus CLNamedPipe::Initialize(const char* pStrPipeName, int flag)
{
	if(pStrPipeName == NULL || strlen(pStrPipeName) == 0)
	{	
		CLLogger::WriteLogMsg("In CLNamedPipe::Initialize(), pStrPipeName is NULL or len is 0", 0);
		return CLStatus(-1, 0);
	}

	m_strPipeName = FILE_PATH_FOR_NAMED_PIPE;
	m_strPipeName += pStrPipeName;

	if((mkfifo(m_strPipeName.c_str(), S_IRUSR | S_IWUSR) == -1) && (errno != EEXIST))
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::Initialize(), mkfifo error", errno);
		return CLStatus(-1, 0);
	}

	m_lAtomWriteSize = pathconf(m_strPipeName.c_str(), _PC_PIPE_BUF);
	if(m_lAtomWriteSize == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::Initialize(), pathconf error", errno);
		m_lAtomWriteSize = 1000;
	}
	if(flag == PIPE_FOR_READ)
	{
		m_Fd = open(m_strPipeName.c_str(), O_RDONLY | O_NONBLOCK);
		if(m_Fd == -1)
		{
			CLLogger::WriteLogMsg("In CLNamedPipe::Initialize(), for read open error", errno);
			return CLStatus(-1, 0);
		}
	}
	else if(flag == PIPE_FOR_WRITE)
	{
		m_Fd = open(m_strPipeName.c_str(), O_WRONLY | O_NONBLOCK);
		if(m_Fd == -1)
		{
			CLLogger::WriteLogMsg("In CLNamedPipe::Initialize(), for write open error", errno);
			return CLStatus(-1, 0);
		}
	}
	else
	{
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLNamedPipe::Read(char *pBuf, int length)
{
	CLCriticalSection cs(m_pMutexForNamedPipe);
	int  readLen = 0;

	readLen = read(m_Fd, pBuf, length);

	if(readLen == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::Read(), read error", errno);
		return CLStatus(-1, errno);
	}

	return CLStatus(readLen, 0);
}

CLStatus CLNamedPipe::Write(char *pBuf, int length)
{
	CLCriticalSection cs(m_pMutexForNamedPipe);
	int writeLen = 0;

	writeLen = write(m_Fd, pBuf, length);

	if(writeLen == -1)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::Write(), write error", errno);
		return CLStatus(-1, errno);
	}

	return CLStatus(writeLen, 0);	
}

long CLNamedPipe::GetAtomWriteSize()
{
	return m_lAtomWriteSize;
}

CLStatus CLNamedPipe::ReadVecs(CLIOVector& dataVec)
{
	CLCriticalSection cs(m_pMutexForNamedPipe);
	int readLen = 0;

	struct iovec *pDataVecs = dataVec.GetIOVecStructs();
	if(pDataVecs == NULL)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::ReadVecs(), dataVec.GetIOVecStructs() error", 0);
		return CLStatus(-1, /*EMPTY_SPACE*/0);
	}

	readLen = readv(m_Fd, pDataVecs, dataVec.IOVecNum());
	if(-1 == readLen)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::ReadVecs(), readv() error", 0);
		return CLStatus(-1, 0);
	}

	delete [] pDataVecs;
	return CLStatus(readLen, 0);
}

CLStatus CLNamedPipe::WriteVecs(CLIOVector& dataVec)
{
	CLCriticalSection cs(m_pMutexForNamedPipe);
	
	int writeLen = 0;
	struct iovec *pDataVecs = dataVec.GetIOVecStructs();
	if(pDataVecs == NULL)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::WriteVecs(), dataVec.GetIOVecStructs() error", 0);
		return CLStatus(-1, /*EMPTY_SPACE*/0);
	}

	writeLen = writev(m_Fd, pDataVecs, dataVec.IOVecNum());
	if(-1 == writeLen)
	{
		CLLogger::WriteLogMsg("In CLNamedPipe::WriteVecs(), writev() error", 0);
		return CLStatus(-1, 0);
	}

	delete [] pDataVecs;
	return CLStatus(writeLen, 0);
}