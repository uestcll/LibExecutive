#include <string.h>
#include "CLMsgLoopManagerForIOMultiplexing.h"

CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(CLMessageObserver *pMsgObserver, const char* pstrThreadName) : CLMessageLoopManager(pMsgObserver)
{
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLMsgLoopManagerForIOMultiplexing::CLMsgLoopManagerForIOMultiplexing(), pstrThreadName error";

	m_strThreadName = pstrThreadName;
}

CLMsgLoopManagerForIOMultiplexing::~CLMsgLoopManagerForIOMultiplexing()
{
}

CLStatus CLMsgLoopManagerForIOMultiplexing::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMsgLoopManagerForIOMultiplexing::WaitForMessage()
{
	return CLStatus(0, 0);
}