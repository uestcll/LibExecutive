#include "CLExecutiveCommunication.h"
#include "CLMessageSerializer.h"

CLExecutiveCommunication::CLExecutiveCommunication(CLMessageSerializer *pMsgSerializer)
{
	m_pMsgSerializer = pMsgSerializer;
}

CLExecutiveCommunication::~CLExecutiveCommunication()
{
	if(m_pMsgSerializer)
		delete m_pMsgSerializer;
}