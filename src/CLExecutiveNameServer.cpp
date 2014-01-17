#include <string.h>
#include "CLExecutiveNameServer.h"
#include "CLCriticalSection.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "CLMessagePoster.h"
#include "CLMessage.h"
#include "CLMutex.h"
#include "CLDataPostResultNotifier.h"
#include "CLProtocolDataPoster.h"

CLExecutiveNameServer* CLExecutiveNameServer::m_pNameServer = 0;
pthread_mutex_t CLExecutiveNameServer::m_Mutex = PTHREAD_MUTEX_INITIALIZER;

CLExecutiveNameServer::CLExecutiveNameServer()
{
}

CLExecutiveNameServer::~CLExecutiveNameServer()
{
}

CLStatus CLExecutiveNameServer::PostExecutiveMessage(const char* pstrExecutiveName, CLMessage *pMessage, bool bDeleteMsg)
{
	if(pMessage == 0)
		return CLStatus(-1, NORMAL_ERROR);

	try
	{
		if((pstrExecutiveName == 0) || (strlen(pstrExecutiveName) == 0))
			throw CLStatus(-1, NORMAL_ERROR);

		CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
		if(pNameServer == 0)
		{
			CLLogger::WriteLogMsg("In CLExecutiveNameServer::PostExecutiveMessage(), CLExecutiveNameServer::GetInstance error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}

		CLMessagePoster* pComm = pNameServer->GetCommunicationPtr(pstrExecutiveName);
		if(pComm == 0)
		{
			CLLogger::WriteLogMsg("In CLExecutiveNameServer::PostExecutiveMessage(), pNameServer->GetCommunicationPtr error", 0);
			throw CLStatus(-1, NORMAL_ERROR);
		}	

		CLStatus s = pComm->PostMessage(pMessage, new CLDataPostResultNotifier(bDeleteMsg), new CLProtocolDataPoster());
		
		if(!(pNameServer->ReleaseCommunicationPtr(pstrExecutiveName).IsSuccess()))
			CLLogger::WriteLogMsg("In CLExecutiveNameServer::PostExecutiveMessage(), pNameServer->ReleaseCommunicationPtr error", 0);

		if(!s.IsSuccess() && (s.m_clErrorCode == MSG_POST_ERROR))
			CLLogger::WriteLogMsg("In CLExecutiveNameServer::PostExecutiveMessage(), pComm->PostMessage error", 0);

		return s;
	}
	catch(CLStatus& s)
	{
		delete pMessage;
		return s;
	}
}

CLStatus CLExecutiveNameServer::Register(const char* strExecutiveName, CLMessagePoster *pExecutiveCommunication)
{
	if(pExecutiveCommunication == 0)
		return CLStatus(-1, 0);

	if((strExecutiveName == 0) || (strlen(strExecutiveName) == 0))
	{
		delete pExecutiveCommunication;
		return CLStatus(-1, 0);
	}

	CLMutex mutex(&m_Mutex);	
	CLCriticalSection cs(&mutex);

	if(m_pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLExecutiveNameServer::Register(), m_pNameServer is 0", 0);
		return CLStatus(-1, 0);
	}
	
	std::map<std::string, SLExecutiveCommunicationPtrCount*>::iterator it = m_NameTable.find(strExecutiveName);	
	if(it != m_NameTable.end())
	{
		delete pExecutiveCommunication;
		CLLogger::WriteLogMsg("In CLExecutiveNameServer::Register(), m_NameTable.find error", 0);
		return CLStatus(-1, 0);
	}
	
	SLExecutiveCommunicationPtrCount *p = new SLExecutiveCommunicationPtrCount;
	p->pMsgPoster = pExecutiveCommunication;
	p->nCount = 1;
	
	m_NameTable[strExecutiveName] = p;
	
	return CLStatus(0, 0);
}

CLMessagePoster* CLExecutiveNameServer::GetCommunicationPtr(const char* strExecutiveName)
{
	if((strExecutiveName == 0) || (strlen(strExecutiveName) == 0))
		return 0;
	
	CLMutex mutex(&m_Mutex);	
	CLCriticalSection cs(&mutex);

	if(m_pNameServer == 0)
	{
		CLLogger::WriteLogMsg("In CLExecutiveNameServer::GetCommunicationPtr(), m_pNameServer is 0", 0);
		return 0;
	}

	std::map<std::string, SLExecutiveCommunicationPtrCount*>::iterator it = m_NameTable.find(strExecutiveName);
	if(it == m_NameTable.end())
	{
		CLLogger::WriteLogMsg("In CLExecutiveNameServer::GetCommunicationPtr(), m_NameTable.find error", 0);
		return 0;
	}

	it->second->nCount++;

	return it->second->pMsgPoster;
}

CLStatus CLExecutiveNameServer::ReleaseCommunicationPtr(const char* strExecutiveName)
{
	if((strExecutiveName == 0) || (strlen(strExecutiveName) == 0))
		return CLStatus(-1, 0);

	CLMessagePoster *pTmp = 0;
	
	{
		CLMutex mutex(&m_Mutex);	
		CLCriticalSection cs(&mutex);

		if(m_pNameServer == 0)
		{
			CLLogger::WriteLogMsg("In CLExecutiveNameServer::ReleaseCommunicationPtr(), m_pNameServer is 0", 0);
			return CLStatus(-1, 0);
		}

		std::map<std::string, SLExecutiveCommunicationPtrCount*>::iterator it = m_NameTable.find(strExecutiveName);
		if(it == m_NameTable.end())
		{
			CLLogger::WriteLogMsg("In CLExecutiveNameServer::ReleaseCommunicationPtr(), m_NameTable.find error", 0);
			return CLStatus(-1, 0);
		}

		it->second->nCount--;

		if(it->second->nCount == 0)
		{
			pTmp = it->second->pMsgPoster;
			delete it->second;
			m_NameTable.erase(it);
		}
	}

	if(pTmp != 0)
	{
		if(!(pTmp->Uninitialize(0).IsSuccess()))
			CLLogger::WriteLogMsg("In CLExecutiveNameServer::ReleaseCommunicationPtr(),pTmp->Uninitialize error", 0);

		delete pTmp;
	}
		
	return CLStatus(0, 0);
}

CLExecutiveNameServer* CLExecutiveNameServer::GetInstance()
{
	return m_pNameServer;
}

CLStatus CLExecutiveNameServer::Create()
{
	if(m_pNameServer == 0)
		m_pNameServer = new CLExecutiveNameServer;

	return CLStatus(0, 0);
}

CLStatus CLExecutiveNameServer::Destroy()
{
	if(m_pNameServer == 0)
		return CLStatus(0, 0);

	CLMutex mutex(&m_Mutex);
	CLCriticalSection cs(&mutex);
	
	delete m_pNameServer;

	m_pNameServer = 0;

	return CLStatus(0, 0);
}