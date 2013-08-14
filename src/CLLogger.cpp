#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CLLogger.h"

#define LOG_FILE_NAME "logger"
#define MAX_SIZE 265

CLLogger* CLLogger::m_pLog = 0;
pthread_mutex_t CLLogger::m_Mutex = PTHREAD_MUTEX_INITIALIZER;

CLLogger::CLLogger()
{
	m_Fd = open(LOG_FILE_NAME, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR); 
	if(m_Fd == -1)
		throw "In CLLogger::CLLogger(), open error";
}

CLLogger::~CLLogger()
{
	close(m_Fd);
}

CLStatus CLLogger::WriteLogMsg(const char *pstrMsg, long lErrorCode)
{
	CLLogger *pLog = CLLogger::GetInstance();
	if(pLog == 0)
		return CLStatus(-1, 0);
	
	CLStatus s = pLog->WriteLog(pstrMsg, lErrorCode);
	if(s.IsSuccess())
		return CLStatus(0, 0);
	else
		return CLStatus(-1, 0);
}

CLStatus CLLogger::WriteLog(const char *pstrMsg, long lErrorCode)
{
	if((pstrMsg == 0) || (strlen(pstrMsg) == 0))
		return CLStatus(-1, 0);

	char buf[MAX_SIZE];
	snprintf(buf, MAX_SIZE, "	Error code: %ld\r\n", lErrorCode);

	int r = pthread_mutex_lock(&m_Mutex);
	if(r != 0)
		return CLStatus(-1, r);

	try
	{
		if(m_pLog == 0)
			throw CLStatus(-1, 0);

		throw WriteMsgAndErrcodeToFile(m_Fd, pstrMsg, buf);
	}
	catch(CLStatus& s)
	{
		r = pthread_mutex_unlock(&m_Mutex);
		if(r != 0)
			return CLStatus(-1, r);

		return s;
	}

	return CLStatus(0, 0);
}

CLStatus CLLogger::WriteMsgAndErrcodeToFile(int fd, const char *pstrMsg, const char *pstrErrcode)
{
	int len = strlen(pstrErrcode) + strlen(pstrMsg) + 1;
	char *p = new char[len];
	memset(p, 0, len);
	strcat(p, pstrMsg);
	strcat(p, pstrErrcode);

	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_END;
	lock.l_len = 0;

	if(fcntl(fd, F_SETLKW, &lock) == -1)
    {
        delete [] p;
        return CLStatus(-1, 0);
    }

	ssize_t writedbytes = write(fd, p, len - 1);
	if(writedbytes == -1)
		writedbytes = 0;

	lock.l_type = F_UNLCK;
	lock.l_start = -writedbytes;
	lock.l_whence = SEEK_END;
	lock.l_len = 0;

	fcntl(fd, F_SETLKW, &lock);

	delete [] p;

	return CLStatus(0, 0);
}

CLLogger* CLLogger::GetInstance()
{
	return m_pLog;
}

CLStatus CLLogger::Create()
{
	if(m_pLog != 0)
		return CLStatus(0, 0);

	m_pLog = new CLLogger();

	return CLStatus(0, 0);
}

CLStatus CLLogger::Destroy()
{
	if(m_pLog == 0)
		return CLStatus(0, 0);

	int r = pthread_mutex_lock(&m_Mutex);
	if(r != 0)
		return CLStatus(-1, r);

	delete m_pLog;

	m_pLog = 0;

	r = pthread_mutex_unlock(&m_Mutex);
	if(r != 0)
		return CLStatus(-1, r);

	return CLStatus(0, 0);
}