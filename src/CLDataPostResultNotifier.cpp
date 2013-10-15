#include "CLDataPostResultNotifier.h"
#include "ErrorCode.h"
#include "CLMessage.h"

CLDataPostResultNotifier::CLDataPostResultNotifier()
{
	m_bDeleteMsg = false;
}

CLDataPostResultNotifier::CLDataPostResultNotifier(bool bDeleteMsg)
{
	m_bDeleteMsg = bDeleteMsg;
}

CLDataPostResultNotifier::~CLDataPostResultNotifier()
{
}

void CLDataPostResultNotifier::SetMsg(CLMessage *pMsg)
{
	m_pMsg = pMsg;
}

CLStatus CLDataPostResultNotifier::Notify(int Result)
{
	try
	{
		if(Result == MSG_POST_SUCCESS)
		{
			throw NotifySuccess();
		}
		else if(Result == MSG_POST_ERROR)
		{
			throw NotifyFailure();
		}

		throw CLStatus(-1, NORMAL_ERROR);
	}
	catch(CLStatus& s)
	{
		if((m_pMsg) && (m_bDeleteMsg))
			delete m_pMsg;

		return s;
	}
}

CLStatus CLDataPostResultNotifier::NotifySuccess()
{
	return CLStatus(0, 0);
}

CLStatus CLDataPostResultNotifier::NotifyFailure()
{
	return CLStatus(0, 0);
}