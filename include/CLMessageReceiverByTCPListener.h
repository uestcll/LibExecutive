#ifndef CLMESSAGERECEIVERBYTCPLISTENER_H
#define CLMESSAGERECEIVERBYTCPLISTENER_H

#include "CLMessageReceiver.h"

class CLMessageReceiverByTCPListener : public CLMessageReceiver
{
public:
	CLMessageReceiverByTCPListener(const char *pHostNameOrIp, const char *pServiceNameOrPort, bool isBlock = true, int listenNum = MAX_LISTEN_NUM);

private:
	CLMessageReceiverByTCPListener(const CLMessageReceiverByTCPListener&);
	CLMessageReceiverByTCPListener& operator=(const CLMessageReceiverByTCPListener&);
};

#endif

