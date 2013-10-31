#include "CLDataPoster.h"
#include "CLIOVector.h"
#include "CLProtocolDataPoster.h"

CLDataPoster::CLDataPoster()
{

}

CLDataPoster::~CLDataPoster()
{

}

CLStatus CLDataPoster::SetProtocolDataPoster(CLProtocolDataPoster *pProtoDataPoster)
{
	m_pProtoDataPoster = pProtoDataPoster;

	return CLStatus(0, 0);
}