#include "CLDataReceiverByTimerFd.h"
#include "CLStatus.h"
#include "CLIOVector.h"

using namespace std;

CLDataReceiverByTimerFd::CLDataReceiverByTimerFd(int fd, CLTimerApplyMsg *pMsg)
{
	m_Fd = fd;
	m_strRemoteName = string(pMsg->m_strClaimerName);
	m_iEchoID = pMsg->m_iEchoID;

}

CLDataReceiverByTimerFd::~CLDataReceiverByTimerFd()
{

}

CLStatus CLDataReceiverByTimerFd::GetData(CLIOVector &IOVec)
{
	

	return CLStatus(0, 0);
}

const int CLDataReceiverByTimerFd::GetFd()
{
	return m_Fd;
}