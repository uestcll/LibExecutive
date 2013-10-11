#include "CLDataPoster.h"
#include "CLLogger.h"
#include "ErrorCode.h"

CLDataPoster::CLDataPoster()
{
	m_pProtocolDataPoster = 0;
}

CLDataPoster::~CLDataPoster()
{
}

CLStatus CLDataPoster::PostDelayedData(CLIOVectors *pIOVectors)
{
	CLLogger::WriteLogMsg("In CLDataPoster::PostDelayedData(), not implement error", 0);
	return CLStatus(-1, NORMAL_ERROR);
}

void CLDataPoster::SetProtocolDataPoster(CLProtocolDataPoster *pProtocolDataPoster)
{
	m_pProtocolDataPoster = pProtocolDataPoster;
}