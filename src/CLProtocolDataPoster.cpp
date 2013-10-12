#include "CLProtocolDataPoster.h"
#include "CLEvent.h"
#include "CLDataPostResultNotifier.h"
#include "CLDataPoster.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "CLIOVectors.h"
#include "CLIteratorForIOVectors.h"

CLProtocolDataPoster::CLProtocolDataPoster()
{
	m_pDataPoster = 0;
	m_pEvent = 0;
	m_pResultNotifer = 0;
	
	m_pIOVectors = 0;
	m_pIterProtocolData = new CLIteratorForIOVectors;
	m_LeftLength = 0;

	m_bSetParameters = false;
}

CLProtocolDataPoster::~CLProtocolDataPoster()
{
	if(m_pDataPoster)
		delete m_pDataPoster;

	if(m_pIterProtocolData)
		delete m_pIterProtocolData;

	if(m_pIOVectors)
		delete m_pIOVectors;

	if(m_pResultNotifer)
		delete m_pResultNotifer;
}

CLStatus CLProtocolDataPoster::SetParameters(CLDataPoster *pDataPoster, CLDataPostResultNotifier *pResultNotifier, CLEvent *pEvent)
{
	m_pDataPoster = pDataPoster;
	m_pEvent = pEvent;
	m_pResultNotifer = pResultNotifier;

	m_bSetParameters = true;
}

bool CLProtocolDataPoster::IsSetParameters()
{
	return m_bSetParameters;
}

CLStatus CLProtocolDataPoster::PostProtocolData(CLIOVectors *pIOVectors)
{
	CLStatus s = m_pDataPoster->PostData(pIOVectors);
	if(!s.IsSuccess())
	{
		if(s.m_clErrorCode == DATA_POSTER_POST_PENDING)
		{
			m_pIOVectors = pIOVectors;
			m_pIOVectors->GetIterator(0, *m_pIterProtocolData);
			m_LeftLength = pIOVectors->Size();
			return s;
		}
		else
		{
			CLLogger::WriteLogMsg("In CLProtocolDataPoster::PostProtocolData(), m_pDataPoster->PostData error", 0);

			m_pResultNotifer->Notify(DATA_POSTER_POST_ERROR);
			delete pIOVectors;
			delete this;
			return s;
		}
	}

	if(s.m_clReturnCode == pIOVectors->Size())
	{
		int result = DATA_POSTER_POST_SUCCESS;
		if(m_pEvent)
		{
			if(!m_pEvent->Set().IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLProtocolDataPoster::PostProtocolData(), m_pEvent->Set error", 0);
				result = DATA_POSTER_POST_ERROR;
			}
		}

		m_pResultNotifer->Notify(result);
		delete pIOVectors;
		delete this;

		if(result == DATA_POSTER_POST_ERROR)
			return CLStatus(-1, result);
		else
			return CLStatus(0, 0);
	}

	if(s.m_clReturnCode < pIOVectors->Size())
	{
		m_pIOVectors = pIOVectors;
		m_pIOVectors->GetIterator(s.m_clReturnCode, *m_pIterProtocolData);
		m_LeftLength = pIOVectors->Size() - s.m_clReturnCode;

		m_pResultNotifer->Notify(DATA_POSTER_POST_PARTIAL);
		return CLStatus(-1, DATA_POSTER_POST_PARTIAL);
	}

	return CLStatus(-1, DATA_POSTER_POST_ERROR);
}

CLStatus CLProtocolDataPoster::PostLeftProtocolData()
{
	if(m_LeftLength == 0)
		return CLStatus(0, 0);

	CLIOVectors tmp_iov;
	m_pIOVectors->PushBackRangeToAIOVector(tmp_iov, *m_pIterProtocolData, m_LeftLength);

	CLStatus s = m_pDataPoster->PostDelayedData(&tmp_iov);
	if(!s.IsSuccess())
	{
		if(s.m_clErrorCode == DATA_POSTER_POST_PENDING)
			return s;
		
		m_pResultNotifer->Notify(DATA_POSTER_POST_ERROR);
		delete this;
		return s;
	}

	if(s.m_clReturnCode == m_LeftLength)
	{
		int result = DATA_POSTER_POST_SUCCESS;
		if(m_pEvent)
		{
			if(!m_pEvent->Set().IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLProtocolDataPoster::PostLeftProtocolData(), m_pEvent->Set error", 0);
				result = DATA_POSTER_POST_ERROR;
			}
		}

		m_pResultNotifer->Notify(result);
		delete this;

		if(result == DATA_POSTER_POST_ERROR)
			return CLStatus(-1, result);
		else
			return CLStatus(0, 0);
	}
	
	if(s.m_clReturnCode < m_LeftLength)
	{
		m_pIterProtocolData->Add(s.m_clReturnCode);
		m_LeftLength = m_LeftLength - s.m_clReturnCode;

		m_pResultNotifer->Notify(DATA_POSTER_POST_PARTIAL);

		return CLStatus(-1, DATA_POSTER_POST_PARTIAL);
	}

	return CLStatus(-1, DATA_POSTER_POST_ERROR);
}