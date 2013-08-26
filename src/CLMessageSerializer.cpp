#include "CLMessageSerializer.h"

CLMessageSerializer::CLMessageSerializer()
{
	m_pNextSerializer = NULL;
}

CLMessageSerializer::CLMessageSerializer(CLMessageSerializer *pMsgSerializer)
{
	m_pNextSerializer = pMsgSerializer;
}

CLMessageSerializer::~CLMessageSerializer()
{
	if(m_pNextSerializer)
		delete m_pNextSerializer;
}