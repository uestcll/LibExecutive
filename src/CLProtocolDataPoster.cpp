#include "CLProtocolDataPoster.h"
#include "CLLogger.h"
#include "CLEvent.h"

using namespace std;

CLProtocolDataPoster::CLProtocolDataPoster() : m_topMsgBufIndex(0), m_pDataPoster(0), m_bDataLeftFlag(false)
{

}

CLProtocolDataPoster::~CLProtocolDataPoster()
{

}

CLStatus CLProtocolDataPoster::SetDataPoster(CLDataPoster *pDataPoster)
{
	m_pDataPoster = pDataPoster;

	return CLStatus(0, 0);
}

CLStatus CLProtocolDataPoster::SetEventNotify(CLEvent *pEvent)
{
	m_pEvent = pEvent;

	return CLStatus(0, 0);
}

CLStatus CLProtocolDataPoster::PostProtoData(CLIOVector *pDataVec)//this dataVec must be one Msg`s buffer
{
	if(!m_bDataLeftFlag)
	{
		return PostData(pDataVec);
	}
	else
	{
		m_MsgBufVecList.push_back(pDataVec);
		
		return PostLeftProtoData();
	}

	return CLStatus(0, 0);
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
		if(m_pEvent != NULL)
		{
			CLStatus s2 = m_pEvent->Set();
			if(!s2.IsSuccess())
				return CLStatus(-1, POST_DATA_ERROR);
		}
		pDataVec->FreeAndPopAll();
		delete pDataVec;

		return CLStatus(0, POST_DATA_COMPLETE);
	}
	else if(s1.m_clReturnCode < pDataVec->Length())
	{
		m_topMsgBufIndex = s1.m_clReturnCode;
	
		m_MsgBufVecList.push_front(pDataVec);

		m_bDataLeftFlag = true;
		
		return CLStatus(-1, POST_DATA_PARTION);
	}

	return CLStatus(0, 0);
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
			if(m_MsgBufVecList.empty())
			{
				m_bDataLeftFlag = false;
				return s;
			}
		}
		else
		{
			return s;
		}
	}
}


