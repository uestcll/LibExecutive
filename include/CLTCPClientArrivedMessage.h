#ifndef CLTCPClientArrivedMessage_h
#define CLTCPClientArrivedMessage_h

#include "CLMessage.h"
#include "MessageID.h"

class CLSocket;
class CLSocketAddress;


class CLTCPClientArrivedMessage : public CLMessage
{
public:
	CLTCPClientArrivedMessage() : CLMessage(TCP_CLIENT_ARRIVED_MSG_ID)
	{

	}

	virtual ~CLTCPClientArrivedMessage()
	{
		if(m_pClientSocket)
		{
			delete m_pClientSocket;
			m_pClientSocket = NULL;
		}
		if(m_pClientAddr)
		{
			delete m_pClientAddr;
			m_pClientAddr = NULL;
		}
	}

public:
	CLSocket *m_pClientSocket;
	CLSocketAddress *m_pClientAddr;
};

#endif