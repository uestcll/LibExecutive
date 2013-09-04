#include "CLMessagePoster.h"
#include "CLMessageSerializer.h"
#include "CLProtocolEncapsulator.h"

CLMessagePoster::CLMessagePoster(CLProtocolEncapsulator *pProtocolEncapsulator)
{
	if(pProtocolEncapsulator == NULL)
		throw "In CLMessagePoster::CLMessagePoster(), pProtocolEncapsulator == NULL";

	m_pCommonSerializer = NULL;
	m_pProtocolEncapsulator = pProtocolEncapsulator;
}

CLMessagePoster::CLMessagePoster(CLMessageSerializer *pCommonMsgSerializer, CLProtocolEncapsulator *pProtocolEncapsulator)
{
	if((pProtocolEncapsulator == NULL) ||(pCommonMsgSerializer == NULL))
		throw "In CLMessagePoster::CLMessagePoster(), pProtocolEncapsulator == NULL or pCommonMsgSerializer == NULL";

	m_pCommonSerializer = pCommonMsgSerializer;
	m_pProtocolEncapsulator = pProtocolEncapsulator;
}

CLMessagePoster::~CLMessagePoster()
{
	if(m_pProtocolEncapsulator)
		delete m_pProtocolEncapsulator;

	if(m_pCommonSerializer)
		delete m_pCommonSerializer;
	else
	{
		map<unsigned long, CLMessageSerializer*>::iterator it;
		for(it = m_SerializerTable.begin(); it != m_SerializerTable.end(); it++)
			delete it->second;
	}
}