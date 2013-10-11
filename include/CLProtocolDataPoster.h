#ifndef CLProtocolDataPoster_H
#define CLProtocolDataPoster_H

#include "CLStatus.h"

class CLEvent;
class CLDataPoster;
class CLPostResultNotifier;
class CLIOVectors;
class CLIteratorForIOVectors;

class CLProtocolDataPoster
{
public:
	CLProtocolDataPoster(CLDataPoster *pDataPoster, CLPostResultNotifier *pResultNotifier, CLEvent *pEvent);
	virtual ~CLProtocolDataPoster();

	CLStatus Initialize();
	CLStatus NotifyIntialFinished(bool bResult);
	CLStatus Uninitialize();

	CLStatus PostProtocolData(CLIOVectors *pIOVectors);
	CLStatus PostLeftProtocolData();
	CLStatus NotifyPostFinished(bool bResult);

private:
	CLProtocolDataPoster(const CLProtocolDataPoster&);
	CLProtocolDataPoster& operator=(const CLProtocolDataPoster&);

private:
	CLPostResultNotifier *m_pResultNotifer;
	CLDataPoster *m_pDataPoster;
	CLIOVectors *m_pIOVectors;
	CLIteratorForIOVectors *m_pIterProtocolData;

	CLEvent *m_pEvent;
};

#endif