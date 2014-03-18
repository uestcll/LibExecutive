#include "CLDataReceiverBySTLqueue.h"
#include "CLIOVectors.h"
#include "CLSTLqueue.h"
#include "ErrorCode.h"
#include "CLLogger.h"

using namespace std;

CLDataReceiverBySTLqueue::CLDataReceiverBySTLqueue(CLSTLqueue *pSTLqueue)
{
	if(pSTLqueue == 0)
		throw "In CLDataReceiverBySTLqueue::CLDataReceiverBySTLqueue(), pSTLqueue == 0";

	m_pSTLqueue = pSTLqueue;
}

CLDataReceiverBySTLqueue::~CLDataReceiverBySTLqueue()
{
}

CLStatus CLDataReceiverBySTLqueue::GetData(CLIOVectors& IOVectors, void *pContext)
{
	*((long *)pContext) = 0;

	CLStatus s = m_pSTLqueue->PopData(IOVectors);
	if((!s.IsSuccess()) && (s.m_clErrorCode == NORMAL_ERROR))
		CLLogger::WriteLogMsg("In CLDataReceiverBySTLqueue::GetData(), m_pSTLqueue->PopData error", 0);

	return s;
}

CLUuid CLDataReceiverBySTLqueue::GetUuid()
{
	return m_pSTLqueue->GetUuid();
}