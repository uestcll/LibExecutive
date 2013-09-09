#ifndef CLProtocolDataPoster_H
#define CLProtocolDataPoster_H

#include "CLStatus.h"

class CLEvent;
class CLDataPoster;
class CLPostResultNotifier;
class CLIOVectors;

class CLProtocolDataPoster
{
public:
	CLProtocolDataPoster(CLDataPoster *pDataPoster, CLPostResultNotifier *pResultNotifier = 0, CLEvent *pEvent = 0);
	virtual ~CLProtocolDataPoster();

	CLStatus Initialize();
	CLStatus NotifyIntialFinished(bool bResult);
	CLStatus Uninitialize();

	CLStatus PostProtocolData(CLIOVectors *pIOVectors);
	CLStatus NotifyPostFinished(bool bResult);

private:
	CLProtocolDataPoster(const CLProtocolDataPoster&);
	CLProtocolDataPoster& operator=(const CLProtocolDataPoster&);

private:
	CLEvent *m_pEvent;
	CLPostResultNotifier *m_pResultNotifer;
	CLDataPoster *m_pDataPoster;
	CLIOVectors *m_pIOVectors;
};

#endif