#ifndef CLLogger_H
#define CLLogger_H

#include <pthread.h>
#include "CLStatus.h"

/*
用于向文件LOG_FILE_NAME中，记录日志信息
*/
class CLLogger
{
public:
	static CLLogger* GetInstance();
	static CLStatus WriteLogMsg(const char *pstrMsg, long lErrorCode);
	static CLStatus WriteLogDirectly(const char *pstrMsg, long lErrorCode);

	CLStatus WriteLog(const char *pstrMsg, long lErrorCode);
	CLStatus Flush();

	friend class CLLibExecutiveInitializer;

private:
	static CLStatus Destroy();
	static CLStatus Create();

	static int WriteOfProcessSafety(int fd, const void *buff, size_t nbytes);
	static CLStatus WriteMsgAndErrcodeToFile(int fd, const char *pstrMsg, const char *pstrErrcode);

private:
	CLLogger(const CLLogger&);
	CLLogger& operator=(const CLLogger&);

	CLLogger();
	~CLLogger();

private:
	static CLLogger *m_pLog;
	//多个线程在写日志
	//当一个线程在调用Destroy，另一个线程在试图写日志
	static pthread_mutex_t m_Mutex;

private:
	int m_Fd;
	char *m_pLogBuffer;
	unsigned int m_nUsedBytesForBuffer;
};

#endif