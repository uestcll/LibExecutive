#include "CLDataPostResultNotifier.h"
#include "CLMessage.h"
#include "CLLogger.h"

CLDataPostResultNotifier::CLDataPostResultNotifier(bool isDataDel) : m_bDataDelFlag(isDataDel), m_pMsg(NULL)
{
	
}

CLDataPostResultNotifier::~CLDataPostResultNotifier()
{
	// if(m_pMsg)
	// {
	// 	delete m_pMsg;
	// 	m_pMsg = 0;
	// }
}

CLStatus CLDataPostResultNotifier::SetMessage(CLMessage *pMsg)
{
	m_pMsg = pMsg;

	return CLStatus(0, 0);
}

CLStatus CLDataPostResultNotifier::NotifyResult(int result)
{
	if(m_bDataDelFlag && m_pMsg)
	{
		delete m_pMsg;
		m_pMsg = 0;		
	}

	if(POST_MSG_SUCCESS == result)
	{	
		return NotifySuccess();
	}
	else if(POST_MSG_ERROR == result)
	{
		return NotifyFailure();
	}	

	return CLStatus(0, 0);
}

CLStatus CLDataPostResultNotifier::NotifySuccess()
{

	return CLStatus(0, 0);
}

CLStatus CLDataPostResultNotifier::NotifyFailure()
{
	return CLStatus(0, 0);
}
