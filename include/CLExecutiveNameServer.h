#ifndef CLExecutiveNameServer_H
#define CLExecutiveNameServer_H

#include <map>
#include <string>
#include <pthread.h>
#include "CLStatus.h"

class CLExecutiveCommunication;
class CLMessage;

struct SLExecutiveCommunicationPtrCount
{
	CLExecutiveCommunication *pExecutiveCommunication;
	unsigned int nCount;
};

class CLExecutiveNameServer
{
public:
	static CLExecutiveNameServer* GetInstance();
	static CLStatus PostExecutiveMessage(const char* pstrExecutiveName, CLMessage *pMessage);
	
	friend class CLLibExecutiveInitializer;

public:
	CLStatus Register(const char* strExecutiveName, CLExecutiveCommunication *pExecutiveCommunication);
	CLExecutiveCommunication* GetCommunicationPtr(const char* strExecutiveName);
	CLStatus ReleaseCommunicationPtr(const char* strExecutiveName);

private:
	static CLStatus Create();
	static CLStatus Destroy();

private:
	/*
	出错时，构造函数和析构函数可能会产生字符串类型异常
	*/
	CLExecutiveNameServer();
	virtual ~CLExecutiveNameServer();

	CLExecutiveNameServer(const CLExecutiveNameServer&);
	CLExecutiveNameServer& operator=(const CLExecutiveNameServer&);

private:
	static CLExecutiveNameServer *m_pNameServer;
	static pthread_mutex_t m_Mutex;
	
private:
	std::map<std::string, SLExecutiveCommunicationPtrCount*> m_NameTable;
};

#endif
