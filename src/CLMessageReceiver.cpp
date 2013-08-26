#include "CLMessageReceiver.h"
#include "CLMessageDeserializer.h"

CLMessageReceiver::CLMessageReceiver(CLMessageDeserializer *pMsgDeserializer)
{
	m_pMsgDeserializer = pMsgDeserializer;
}

CLMessageReceiver::~CLMessageReceiver()
{
	if(m_pMsgDeserializer)
		delete m_pMsgDeserializer;
}