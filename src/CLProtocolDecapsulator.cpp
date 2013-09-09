#include "CLProtocolDecapsulator.h"

CLProtocolDecapsulator::CLProtocolDecapsulator(CLProtocolDecapsulator *pProtocolDecapsulator)
{
	m_pProtocolDecapsulator = pProtocolDecapsulator;
}

CLProtocolDecapsulator::~CLProtocolDecapsulator()
{
	if(m_pProtocolDecapsulator)
		delete m_pProtocolDecapsulator;
}