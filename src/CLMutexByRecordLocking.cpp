#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string>
#include "CLMutexByRecordLocking.h"
#include "CLLogger.h"

using namespace std;

#define FILE_PATH_FOR_RECORD_LOCKING "/tmp/"

CLMutexByRecordLocking::CLMutexByRecordLocking(const char *pstrFileName)
{
	if((pstrFileName == 0) || (strlen(pstrFileName) == 0))
		throw "In CLMutexByRecordLocking::CLMutexByRecordLocking(), pstrFileName error";

	m_strFileName = FILE_PATH_FOR_RECORD_LOCKING;
	m_strFileName += pstrFileName;
}

CLMutexByRecordLocking::~CLMutexByRecordLocking()
{
}

CLStatus CLMutexByRecordLocking::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexByRecordLocking::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexByRecordLocking::Lock()
{
	m_Fd = open(m_strFileName.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMsg("In CLMutexByRecordLocking::Lock(), open error", errno);
		return CLStatus(-1, errno);
	}

	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if(fcntl(m_Fd, F_SETLKW, &lock) == -1)
	{
		CLLogger::WriteLogMsg("In CLMutexByRecordLocking::Lock(), fcntl error", errno);
		return CLStatus(-1, errno);
	}

	return CLStatus(0, 0);
}

CLStatus CLMutexByRecordLocking::Unlock()
{
	struct flock lock;
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if(fcntl(m_Fd, F_SETLKW, &lock) == -1)
	{
		CLLogger::WriteLogMsg("In CLMutexByRecordLocking::Unlock(), fcntl error", errno);
		return CLStatus(-1, errno);
	}

	if(close(m_Fd) == -1)
		CLLogger::WriteLogMsg("CLMutexByRecordLocking::Unlock(), close error", errno);

	return CLStatus(0, 0);
}