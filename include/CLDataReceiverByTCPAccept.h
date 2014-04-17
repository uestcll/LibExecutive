#ifndef CLDATARECEIVERBYTCPACCEPT_H
#define CLDATARECEIVERBYTCPACCEPT_H

#include "CLStatus.h"
#include "definition.h"
#include "CLDataReceiver.h"

class CLSocket;

class CLDataReceiverByTCPAccept : public CLDataReceiver
{
public:
	CLDataReceiverByTCPAccept(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock = true, int listenNum = MAX_LISTEN_NUM);
	virtual ~CLDataReceiverByTCPAccept();

	virtual CLStatus GetData(CLBuffer *pBuffer);

	virtual const int GetFd();

private:
	CLSocket *m_pListenSocket;

private:
	CLDataReceiverByTCPAccept(const CLDataReceiverByTCPAccept&);
	CLDataReceiverByTCPAccept& operator=(const CLDataReceiverByTCPAccept&);	
};

#endif