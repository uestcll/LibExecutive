#include "CLDataReceiverByAccept.h"
#include "CLSocket.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "DefinitionForConst.h"

CLDataReceiverByAccept::CLDataReceiverByAccept(CLSocket *pSocket)
{
	if(pSocket == 0)
		throw "In CLDataReceiverByAccept::CLDataReceiverByAccept(), pSocket error";

	m_pSocket = pSocket;
}

CLDataReceiverByAccept::~CLDataReceiverByAccept()
{
	delete m_pSocket;
}

CLStatus CLDataReceiverByAccept::GetData(CLIOVectors& IOVectors, void *pContext)
{
	if(IOVectors.Size() < sizeof(unsigned long))
		return CLStatus(-1, NORMAL_ERROR);

	CLSocket *pConnectedSocket = 0;
	*((long *)pContext) = (long)(m_pSocket->GetSocket());

	CLStatus s = m_pSocket->Accept(&pConnectedSocket);
	if(s.IsSuccess())
	{
		CLStatus s1 = IOVectors.WriteBlock(0, (char *)(&(pConnectedSocket)), sizeof(CLSocket *));
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLDataReceiverByAccept::GetData(), IOVectors.WriteBlock error", 0);
			delete pConnectedSocket;
			return CLStatus(-1, NORMAL_ERROR);
		}

		return CLStatus(sizeof(CLSocket *), 0);
	}

	if(s.m_clErrorCode == ACCEPT_PENDING)
		return CLStatus(-1, RECEIVED_ZERO_BYTE);

	CLLogger::WriteLogMsg("In CLDataReceiverByAccept::GetData(), m_pSocket->Accept error", 0);

	return CLStatus(-1, NORMAL_ERROR);
}

CLUuid CLDataReceiverByAccept::GetUuid()
{
	return m_pSocket->GetUuid();
}

int CLDataReceiverByAccept::GetFd()
{
	return m_pSocket->GetSocket();
}