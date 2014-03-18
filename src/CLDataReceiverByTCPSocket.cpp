#include "CLDataReceiverByTCPSocket.h"
#include "CLSocket.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "DefinitionForConst.h"

CLDataReceiverByTCPSocket::CLDataReceiverByTCPSocket(CLSocket *pSocket)
{
	if(pSocket == 0)
		throw "In CLDataReceiverByTCPSocket::CLDataReceiverByTCPSocket(), pSocket error";

	m_pSocket = pSocket;
}

CLDataReceiverByTCPSocket::~CLDataReceiverByTCPSocket()
{
	delete m_pSocket;
}

CLStatus CLDataReceiverByTCPSocket::GetData(CLIOVectors& IOVectors, void *pContext)
{
	if(IOVectors.Size() == 0)
		return CLStatus(-1, NORMAL_ERROR);

	*((long *)pContext) = (long)(m_pSocket->GetSocket());

	CLStatus s = m_pSocket->Read(IOVectors);
	if(s.m_clReturnCode > 0)
		return s;
	
	if(s.m_clReturnCode == 0)
		return CLStatus(-1, RECEIVED_CLOSE);

	if(s.m_clErrorCode == IO_PENDING)
		return CLStatus(-1, RECEIVED_ZERO_BYTE);

	CLLogger::WriteLogMsg("In CLDataReceiverByTCPSocket::GetData(), m_pSocket->Read error", 0);

	return CLStatus(-1, NORMAL_ERROR);
}

CLUuid CLDataReceiverByTCPSocket::GetUuid()
{
	return m_pSocket->GetUuid();
}