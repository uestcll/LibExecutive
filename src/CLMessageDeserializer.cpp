#include "CLMessageDeserializer.h"

CLMessageDeserializer::CLMessageDeserializer()
{
	m_pNextDeserializer = NULL;
}

CLMessageDeserializer::~CLMessageDeserializer()
{
	if(m_pNextDeserializer)
		delete m_pNextDeserializer;
}

CLMessageDeserializer::CLMessageDeserializer(CLMessageDeserializer *pDeserializer)
{
	m_pNextDeserializer = pDeserializer;
}