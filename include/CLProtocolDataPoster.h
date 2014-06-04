#ifndef CLProtocolDataPoster_H
#define CLProtocolDataPoster_H

#include "CLStatus.h"

class CLDataPostResultNotifier;
class CLNotify;
class CLDataPoster;
class CLIOVectors;
class CLIteratorForIOVectors;

class CLProtocolDataPoster
{
public:
	CLProtocolDataPoster();
	virtual ~CLProtocolDataPoster();

	CLStatus SetParameters(CLDataPoster *pDataPoster, CLDataPostResultNotifier *pResultNotifier, CLNotify *pEvent);
	bool IsSetParameters();

	CLStatus PostProtocolData(CLIOVectors *pIOVectors);
	CLStatus PostLeftProtocolData();

private:
	CLProtocolDataPoster(const CLProtocolDataPoster&);
	CLProtocolDataPoster& operator=(const CLProtocolDataPoster&);

private:
	CLDataPostResultNotifier *m_pResultNotifer;
	CLDataPoster *m_pDataPoster;
	CLNotify *m_pEvent;

	CLIOVectors *m_pIOVectors;
	CLIteratorForIOVectors *m_pIterProtocolData;
	unsigned int m_LeftLength;

	bool m_bSetParameters;
};

#endif
