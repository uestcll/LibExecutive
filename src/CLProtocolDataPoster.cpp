#include "CLProtocolDataPoster.h"
#include "CLLogger.h"
#include "CLEvent.h"
#include "CLDataPoster.h"
#include "CLIOVector.h"
#include "CLDataPostResultNotifier.h"

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
		SLDataAndNotifier *pDataAndNotifier = m_MsgBufVecList.front();
		m_MsgBufVecList.pop_front();

		(pDataAndNotifier->m_pData)->FreeAndPopAll();
		delete (pDataAndNotifier->m_pData);

		delete (pDataAndNotifier->m_pResNotifier);
		delete pDataAndNotifier;
	}
}

// CLStatus CLProtocolDataPoster::FreeDataAndNotifyStruct(SLDataAndNotifier *pDataAndNotifier)
// {

// }

CLStatus CLProtocolDataPoster::SetParameters(CLDataPoster *pDataPoster, CLEvent *pEvent)
{
	m_pEvent = pEvent;
	m_pDataPoster = pDataPoster;

	return CLStatus(0, 0);
}

CLStatus CLProtocolDataPoster::PostProtoData(SLDataAndNotifier *pDataAndNotifier)//this dataVec must be one Msg`s buffer
{
	if(!m_bDataLeftFlag)
	{
		CLStatus s = PostData(pDataAndNotifier->m_pData);
		
		if(s.m_clErrorCode == POST_DATA_PARTION)
		{
			m_MsgBufVecList.push_back(pDataAndNotifier);
		}
		else
		{
			CLStatus s1(0, 0);
			if(s.IsSuccess())
			{
				s1 = (pDataAndNotifier->m_pResNotifier)->NotifyResult(POST_MSG_SUCCESS);			
			}
			else 
				s1 = (pDataAndNotifier->m_pResNotifier)->NotifyResult(POST_MSG_ERROR);	

			if(!s1.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLProtocolDataPoster::PostProtoData(), pDataAndNotifier->NotifyResult() error", 0);
			}

			(pDataAndNotifier->m_pData)->FreeAndPopAll();
			delete (pDataAndNotifier->m_pData);

			delete (pDataAndNotifier->m_pResNotifier);
			delete pDataAndNotifier;
		}

		return s;
	}
	else
	{
		m_MsgBufVecList.push_back(pDataAndNotifier);
		
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
	
		m_bDataLeftFlag = true;
		
		return CLStatus(-1, POST_DATA_PARTION);
	}
}

CLStatus CLProtocolDataPoster::PostLeftProtoData()
{
	if(m_MsgBufVecList.empty())
	{
		m_bDataLeftFlag = false;
		return CLStatus(0, 0);
	}

	while(1)
	{
		CLIOVector partDataVec;	

		SLDataAndNotifier *pTopDataAndNotifier = m_MsgBufVecList.front(); 	
		m_MsgBufVecList.pop_front();

		CLIOVector *pTopDataVec = (pTopDataAndNotifier->m_pData);
		pTopDataVec->GetIOVecs(m_topMsgBufIndex, pTopDataVec->Length(), partDataVec);

		CLStatus s = PostData(&partDataVec);
		if(s.m_clErrorCode == POST_DATA_PARTION)
		{
			m_MsgBufVecList.push_front(pTopDataAndNotifier);
			return s;
		}
		else
		{
			CLStatus s1(0, 0);
			if(s.IsSuccess())
			{
				s1 = (pTopDataAndNotifier->m_pResNotifier)->NotifyResult(POST_MSG_SUCCESS);			
			}
			else 
				s1 = (pTopDataAndNotifier->m_pResNotifier)->NotifyResult(POST_MSG_ERROR);	

			if(!s1.IsSuccess())
			{
				CLLogger::WriteLogMsg("In CLProtocolDataPoster::PostProtoData(), pTopDataAndNotifier->NotifyResult() error", 0);
			}

			pTopDataVec->FreeAndPopAll();
			delete pTopDataVec;
			delete (pTopDataAndNotifier->m_pResNotifier);
			delete pTopDataAndNotifier;

			if(m_MsgBufVecList.empty())
			{
				m_bDataLeftFlag = false;
				return s;
			}
		}
	}
}


