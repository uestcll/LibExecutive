#include "CLProtocolDataPoster.h"
#include "CLEvent.h"
#include "CLPostResultNotifier.h"
#include "CLDataPoster.h"
#include "CLLogger.h"

CLProtocolDataPoster::CLProtocolDataPoster(CLDataPoster *pDataPoster, CLPostResultNotifier *pResultNotifier, CLEvent *pEvent)
{
	m_pDataPoster = pDataPoster;
	m_pEvent = pEvent;
	m_pResultNotifer = pResultNotifier;
}

CLProtocolDataPoster::~CLProtocolDataPoster()
{
}

CLStatus CLProtocolDataPoster::Initialize()
{
	CLStatus s = m_pDataPoster->Initialize();
	if(s.IsSuccess())
	{
		if(m_pResultNotifer)
			m_pResultNotifer->Notify(true);
	}
	else
	{
		if(s.m_clErrorCode == INITIALIZE_ERROR)
		{
			if(m_pResultNotifer)
				m_pResultNotifer->Notify(false);
		}
		else if(s.m_clErrorCode == INITIALIZE_PENDING)
		{
			return s;
		}
	}

	delete m_pResultNotifer;

	delete this;

	return s;
}

CLStatus CLProtocolDataPoster::NotifyIntialFinished(bool bResult)
{
	if(m_pResultNotifer)
		m_pResultNotifer->Notify(bResult);

	delete m_pResultNotifer;

	delete this;

	return CLStatus(0, 0);
}

CLStatus CLProtocolDataPoster::Uninitialize()
{
	CLStatus s = m_pDataPoster->Uninitialize();
	
	delete this;
	
	return s;
}