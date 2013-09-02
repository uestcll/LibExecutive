#include "CLProtocolEncapsulator.h"

CLProtocolEncapsulator::CLProtocolEncapsulator()
{
	m_pNextProtocolEncapsulator = NULL;
}

CLProtocolEncapsulator::CLProtocolEncapsulator(CLProtocolEncapsulator *pProtocolEncapsulator)
{
	m_pNextProtocolEncapsulator = pProtocolEncapsulator;
}

CLProtocolEncapsulator::~CLProtocolEncapsulator()
{
	if(m_pNextProtocolEncapsulator)
		delete m_pNextProtocolEncapsulator;
}