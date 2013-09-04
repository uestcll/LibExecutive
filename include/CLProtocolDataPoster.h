#ifndef CLProtocolDataPoster_H
#define CLProtocolDataPoster_H

#include "CLStatus.h"

class CLEvent;
class CLDataPoster;
class CLPostResultNotifier;

class CLProtocolDataPoster
{
public:
	CLProtocolDataPoster(CLDataPoster *pDataPoster, CLPostResultNotifier *pResultNotifier = 0, CLEvent *pEvent = 0);
	virtual ~CLProtocolDataPoster();

	CLStatus Initialize();
	CLStatus NotifyIntialFinished(bool bResult);
	CLStatus Uninitialize();

private:
	CLProtocolDataPoster(const CLProtocolDataPoster&);
	CLProtocolDataPoster& operator=(const CLProtocolDataPoster&);

private:
	CLEvent *m_pEvent;
	CLPostResultNotifier *m_pResultNotifer;
	CLDataPoster *m_pDataPoster;
};

#endif