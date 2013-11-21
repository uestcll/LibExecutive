#ifndef CLExecutiveNameServer_H
#define CLExecutiveNameServer_H

#include <map>
#include <string>
#include <pthread.h>
#include "CLStatus.h"

class CLMessagePoster;
class CLMessage;
class CLDataPostResultNotifier;

struct SLMessagePosterPtrCount
{
	CLMessagePoster *pMsgPoster;
	unsigned int nCount;
};

class CLExecutiveNameServer
{
public:
	static CLExecutiveNameServer* GetInstance();
	static CLStatus PostExecutiveMessage(const char* pstrExecutiveName, CLMessage *pMessage, CLDataPostResultNotifier *pResNotifier);
	
	friend class CLLibExecutiveInitializer;

public:
	CLStatus Register(const char* strExecutiveName, CLMessagePoster *pMsgPoster);
	CLMessagePoster* GetCommunicationPtr(const char* strExecutiveName);
	CLStatus ReleaseCommunicationPtr(const char* strExecutiveName);

private:
	static CLStatus Create();
	static CLStatus Destroy();

private:
	CLExecutiveNameServer();
	virtual ~CLExecutiveNameServer();

	CLExecutiveNameServer(const CLExecutiveNameServer&);
	CLExecutiveNameServer& operator=(const CLExecutiveNameServer&);

private:
	static CLExecutiveNameServer *m_pNameServer;
	static pthread_mutex_t m_Mutex;
	
private:
	std::map<std::string, SLMessagePosterPtrCount*> m_NameTable;
};

#endif