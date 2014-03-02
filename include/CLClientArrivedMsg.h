#ifndef CLClientArrivedMsg_H
#define CLClientArrivedMsg_H

#include "CLMessage.h"

class CLSocket;

class CLClientArrivedMsg : public CLMessage
{
public:
	CLClientArrivedMsg(CLSocket *pSocket);
	virtual ~CLClientArrivedMsg();

	CLSocket *GetSocket();

private:
	CLClientArrivedMsg(const CLClientArrivedMsg&);
	CLClientArrivedMsg& operator=(const CLClientArrivedMsg&);

private:
	CLSocket *m_pSocket;
};

#endif