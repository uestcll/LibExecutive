#include "CLClientArrivedMsg.h"
#include "DefinitionForConst.h"

CLClientArrivedMsg::CLClientArrivedMsg(CLSocket *pSocket) : CLMessage(MESSAGE_ID_FOR_CLIENT_ARRIVED)
{
	m_pSocket = pSocket;
}

CLClientArrivedMsg::~CLClientArrivedMsg()
{
}

CLSocket *CLClientArrivedMsg::GetSocket()
{
	return m_pSocket;
}