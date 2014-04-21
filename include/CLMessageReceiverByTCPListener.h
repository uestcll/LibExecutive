#ifndef CLMESSAGERECEIVERBYTCPLISTENER_H
#define CLMESSAGERECEIVERBYTCPLISTENER_H

#include "CLMessageReceiver.h"
#include "definition.h"

class CLMessageReceiverByTCPListener : public CLMessageReceiver
{
public:
	CLMessageReceiverByTCPListener(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock = true, int listenNum = MAX_LISTEN_NUM);
	virtual ~CLMessageReceiverByTCPListener();
	
private:
	CLMessageReceiverByTCPListener(const CLMessageReceiverByTCPListener&);
	CLMessageReceiverByTCPListener& operator=(const CLMessageReceiverByTCPListener&);
};

#endif

