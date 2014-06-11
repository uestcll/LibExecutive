#include <memory.h>
#include "CLMessageLoopManager.h"
#include "CLMessageReceiver.h"
#include "CLMessageDeserializer.h"
#include "CLProtocolDecapsulator.h"
#include "CLDataReceiver.h"
#include "CLUuid.h"
#include "CLIOVectors.h"
#include "CLLogger.h"
#include "ErrorCode.h"
#include "CLBufferManager.h"
#include "CLChannelClosedMsg.h"
#include "CLGetMessageErrorMsg.h"

using namespace std;

CLMessageReceiver::CLMessageReceiver(CLUuid *pChannelUuid, CLBufferManager *pBufferManager, CLDataReceiver *pDataReceiver, CLMessageDeserializer *pMsgDeserializer, CLProtocolDecapsulator *pProtocolDecapsulator)
{
	try
	{
		if(pBufferManager == 0)
			throw "In CLMessageReceiver::CLMessageReceiver(), pBufferManager == 0 error";

		if(pDataReceiver == 0)
			throw "In CLMessageReceiver::CLMessageReceiver(), pDataReceiver == 0 error";

		if(pMsgDeserializer == 0)
			throw "In CLMessageReceiver::CLMessageReceiver(), pMsgDeserializer == 0 error";

		m_pDataReceiver = pDataReceiver;
		m_pProtocolDecapsulator = pProtocolDecapsulator;
		m_pMsgDeserializer = pMsgDeserializer;
		m_pBufferManager = pBufferManager;

		m_pChannelUuid = new CLUuid(*pChannelUuid);
	}
	catch(const char *s)
	{
		if(pBufferManager)
			delete pBufferManager;

		if(pDataReceiver)
			delete pDataReceiver;

		if(pProtocolDecapsulator)
			delete pProtocolDecapsulator;

		if(pMsgDeserializer)
			delete pMsgDeserializer;

		throw s;
	}
}

CLMessageReceiver::~CLMessageReceiver()
{
	if(m_pBufferManager)
		m_pBufferManager->SetDestroyFlag();

	if(m_pMsgDeserializer)
		delete m_pMsgDeserializer;

	if(m_pProtocolDecapsulator)
		delete m_pProtocolDecapsulator;

	if(m_pDataReceiver)
		delete m_pDataReceiver;

	if(m_pChannelUuid)
		delete m_pChannelUuid;
}

CLStatus CLMessageReceiver::GetMessage(std::queue<SLMessageAndSource*>& qMsgContainer)
{
	CLIOVectors ReceiveIOVec;
	try
	{
		CLStatus s11 = m_pBufferManager->GetEmptyIOVector(ReceiveIOVec);
		if(!s11.IsSuccess())
		{
			CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pDataReceiver->GetEmptyIOVector error", 0);

			throw s11;
		}

		long Context = 0;
		CLStatus s1 = m_pDataReceiver->GetData(ReceiveIOVec, &Context);
		if(!s1.IsSuccess())
		{
			if(s1.m_clErrorCode == NORMAL_ERROR)
			{
				CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pDataReceiver->GetData error", 0);
				throw CLStatus(-1, NORMAL_ERROR);
			}
			else if(s1.m_clErrorCode == RECEIVED_CLOSE)
			{
				SLMessageAndSource *pMsgInfo = new SLMessageAndSource;
				pMsgInfo->pMsg = new CLChannelClosedMsg;
				pMsgInfo->ChannelUuid = *m_pChannelUuid;

				qMsgContainer.push(pMsgInfo);
				throw CLStatus(0, 0);
			}

			throw s1;
		}

		CLIOVectors DataIOVec;
		m_pBufferManager->GetPartialDataIOVector(DataIOVec);

		unsigned int length = DataIOVec.Size();
		length = length + s1.m_clReturnCode;
		
		DataIOVec.PushBackIOVector(ReceiveIOVec, IOVECTOR_NON_DELETE);

		if(m_pProtocolDecapsulator)
		{
			vector<CLIOVectors *> vSerializedMsgs;
			CLIOVectors IOVectorsForPartialData;

			try
			{
				CLStatus s2 = m_pProtocolDecapsulator->Decapsulate(DataIOVec, length, vSerializedMsgs, IOVectorsForPartialData, &Context);
				if(!s2.IsSuccess())
				{
					CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pProtocolDecapsulator->Decapsulate error", 0);
					throw s2;
				}

				m_pBufferManager->SetPartialDataIOVector(IOVectorsForPartialData);

				for(int i = 0; i < vSerializedMsgs.size(); i++)
				{
					CLMessage *pMsg = 0;
					CLStatus s3 = m_pMsgDeserializer->Deserialize(*(vSerializedMsgs[i]), &pMsg, *m_pBufferManager, &Context);
					if(!s3.IsSuccess() || (pMsg == 0))
					{
						CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pMsgDeserializer->Deserialize or pMsg == 0 error", 0);
						throw s3;
					}

					SLMessageAndSource *pMsgInfo = new SLMessageAndSource;
					pMsgInfo->pMsg = pMsg;
					pMsgInfo->ChannelUuid = *m_pChannelUuid;

					qMsgContainer.push(pMsgInfo);
				}

				throw CLStatus(0, 0);
			}
			catch(CLStatus& s)
			{
				for(int i = 0; i < vSerializedMsgs.size(); i++)
					delete vSerializedMsgs[i];

				throw s;
			}
		}

		CLIOVectors tmp_iov;
		CLStatus s44 = DataIOVec.PushBackRangeToAIOVector(tmp_iov, 0, length);
		if(!s44.IsSuccess() || (s44.m_clReturnCode != length))
		{
			CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), DataIOVec.PushBackRangeToAIOVector error", 0);
			throw s44;
		}

		CLMessage *pMsg1 = 0;
		CLStatus s4 = m_pMsgDeserializer->Deserialize(tmp_iov, &pMsg1, *m_pBufferManager, &Context);
		if(!s4.IsSuccess() || (pMsg1 == 0))
		{
			CLLogger::WriteLogMsg("In CLMessageReceiver::GetMessage(), m_pMsgDeserializer->Deserialize or pMsg1 == 0 error", 0);
			throw s4;
		}

		SLMessageAndSource *pMsgInfo1 = new SLMessageAndSource;
		pMsgInfo1->pMsg = pMsg1;
		pMsgInfo1->ChannelUuid = *m_pChannelUuid;

		qMsgContainer.push(pMsgInfo1);
	}
	catch(CLStatus& s)
	{
		if(!s.IsSuccess())
		{
			SLMessageAndSource *pErrorMsgInfo = new SLMessageAndSource;
			pErrorMsgInfo->pMsg = new CLGetMessageErrorMsg;
			pErrorMsgInfo->ChannelUuid = *m_pChannelUuid;
			qMsgContainer.push(pErrorMsgInfo);

			return CLStatus(-1, 0);
		}
		else
			return CLStatus(0, 0);
	}
}