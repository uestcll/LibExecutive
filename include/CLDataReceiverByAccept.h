#ifndef CLDataReceiverByAccept_H
#define CLDataReceiverByAccept_H

#include "CLDataReceiver.h"

class CLSocket;

class CLDataReceiverByAccept : public CLDataReceiver
{
public:
	CLDataReceiverByAccept(CLSocket *pSocket);
	virtual ~CLDataReceiverByAccept();

	virtual CLStatus GetData(CLIOVectors& IOVectors, void *pContext);
	virtual CLUuid GetUuid();

private:
	CLDataReceiverByAccept(const CLDataReceiverByAccept&);
	CLDataReceiverByAccept& operator=(const CLDataReceiverByAccept&);

private:
	CLSocket *m_pSocket;
};

#endif