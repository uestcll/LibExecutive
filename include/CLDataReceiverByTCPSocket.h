#ifndef CLDataReceiverByTCPSocket_H
#define CLDataReceiverByTCPSocket_H

#include "CLDataReceiver.h"

class CLSocket;

class CLDataReceiverByTCPSocket : public CLDataReceiver
{
public:
	CLDataReceiverByTCPSocket(CLSocket *pSocket);
	virtual ~CLDataReceiverByTCPSocket();

	virtual CLStatus GetData(CLIOVectors& IOVectors, void *pContext);

private:
	CLDataReceiverByTCPSocket(const CLDataReceiverByTCPSocket&);
	CLDataReceiverByTCPSocket& operator=(const CLDataReceiverByTCPSocket&);

private:
	CLSocket *m_pSocket;
};

#endif