#include "CLProtocolDataPoster.h"
#include "CLLogger.h"
#include "CLEvent.h"
#include "CLDataPoster.h"
#include "CLIOVector.h"

using namespace std;

CLProtocolDataPoster::CLProtocolDataPoster() : m_topMsgBufIndex(0), m_pDataPoster(0), m_bDataLeftFlag(false), m_pEvent(0)
{

}

CLProtocolDataPoster::~CLProtocolDataPoster()
{
	if(m_pDataPoster)
	{	
		delete m_pDataPoster;
		m_pDataPoster = 0;
	}
	while(1)
	{
		if(m_MsgBufVecList.empty())
		{
			break;
		}
		CLIOVector *tmpVec = m_MsgBufVecList.front();
		m_MsgBufVecList.pop_front();
		tmpVec->FreeAndPopAll();
		delete tmpVec;
	}
}

CLStatus CLProtocolDataPoster::SetParameters(CLDataPoster *pDataPoster, CLEvent *pEvent)
{
	m_pEvent = pEvent;
	m_pDataPoster = pDataPoster;

	return CLStatus(0, 0);
}

CLStatus CLProtocolDataPoster::PostProtoData(CLIOVector *pDataVec)//this dataVec must be one Msg`s buffer
{
	if(!m_bDataLeftFlag)
	{
		CLStatus s = PostData(pDataVec);
		if(s.IsSuccess())
		{
			pDataVec->FreeAndPopAll();
			delete pDataVec;
		}
		return s;
	}
	else
	{
		m_MsgBufVecList.push_back(pDataVec);
		
		return PostLeftProtoData();
	}
}

CLStatus CLProtocolDataPoster::PostData(CLIOVector *pDataVec)
{
	CLStatus s1 = m_pDataPoster->PostData(pDataVec);
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMsg("In CLProtocolDataPoster::PostProtoData(), m_pDataPoster->PostData() error", 0);
		return s1;
	}
	if(s1.m_clReturnCode == pDataVec->Length())
	{
		m_topMsgBufIndex = 0;
		if(m_pEvent != NULL)
		{
			CLStatus s2 = m_pEvent->Set();
			if(!s2.IsSuccess())
				return CLStatus(-1, POST_DATA_ERROR);
		}
		// pDataVec->FreeAndPopAll();
		// delete pDataVec;

		return CLStatus(0, POST_DATA_COMPLETE);
	}
	else if(s1.m_clReturnCode < pDataVec->Length())
	{
		m_topMsgBufIndex += s1.m_clReturnCode;
	
		//m_MsgBufVecList.push_front(pDataVec);

		m_bDataLeftFlag = true;
		
		return CLStatus(-1, POST_DATA_PARTION);
	}
}

CLStatus CLProtocolDataPoster::PostLeftProtoData()
{
	while(1)
	{
		CLIOVector partDataVec;
		CLIOVector *pTopDataVec = m_MsgBufVecList.front(); 
		m_MsgBufVecList.pop_front();
		pTopDataVec->GetIOVecs(m_topMsgBufIndex, pTopDataVec->Length(), partDataVec);

		CLStatus s = PostData(&partDataVec);
		if(s.IsSuccess())
		{
			pTopDataVec->FreeAndPopAll();
			delete pTopDataVec;
			if(m_MsgBufVecList.empty())
			{
				m_bDataLeftFlag = false;
				return s;
			}
		}
		else if(s.m_clErrorCode == POST_DATA_PARTION)
		{
			m_MsgBufVecList.push_front(pTopDataVec);
			return s;
		}
		else
		{
			return s;
		}
	}
}


