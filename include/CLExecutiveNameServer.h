#ifndef CLExecutiveNameServer_H
#define CLExecutiveNameServer_H

#include <map>
#include <string>
#include <pthread.h>
#include "CLUuid.h"
#include "CLStatus.h"

class CLMessagePoster;
class CLMessageReceiver;
class CLMessage;

struct SLExecutiveCommunicationPtrCount
{
	CLMessagePoster *pMsgPoster;
	CLMessageReceiver *pMsgReceiver;
	unsigned int nCount;
};

struct SLChannelUuidPtrCount
{
	CLUuid ChannelUuid;
	unsigned int nCount;
};

class CLExecutiveNameServer
{
public:
	static CLExecutiveNameServer* GetInstance();
	static CLStatus PostExecutiveMessage(const char* pstrExecutiveName, CLMessage *pMessage, bool bDeleteMsg = false);
	static CLStatus PostExecutiveMessage(CLUuid ChannelUuid, CLMessage *pMessage, bool bDeleteMsg = false);
	
	friend class CLLibExecutiveInitializer;

public:
	CLStatus Register(const char* strExecutiveName, CLUuid ChannelUuid, CLMessagePoster *pExecutiveCommunication, CLMessageReceiver *pMsgReceiver = 0);
	CLMessagePoster* GetCommunicationPtr(const char* strExecutiveName);
	CLStatus ReleaseCommunicationPtr(const char* strExecutiveName);

	CLStatus Register(CLUuid ChannelUuid, CLMessagePoster *pExecutiveCommunication, CLMessageReceiver *pMsgReceiver = 0);
	CLMessagePoster* GetCommunicationPtr(CLUuid ChannelUuid);
	CLStatus ReleaseCommunicationPtr(CLUuid ChannelUuid);

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
	static pthread_mutex_t m_MutexForUuidTable;
	static pthread_mutex_t m_MutexForNameTable;
	
private:
	std::map<std::string, SLChannelUuidPtrCount*> m_NameTable;
	std::map<CLUuid, SLExecutiveCommunicationPtrCount*, CLUuidComparer> m_UuidTable;
};

#endif