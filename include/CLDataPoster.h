#ifndef CLDataPoster_H
#define CLDataPoster_H

#include "CLStatus.h"

class CLIOVectors;
class CLProtocolDataPoste;

class CLDataPoster
{
public:
	CLDataPoster();
	virtual ~CLDataPoster();

	virtual CLStatus Initialize(void *pContext) = 0;
	virtual CLStatus Uninitialize() = 0;

	virtual CLStatus PostData(CLIOVectors *pIOVectors) = 0;
	virtual CLStatus PostDelayedData(CLIOVectors *pIOVectors);

	virtual CLDataPoster* Copy() = 0;

	void SetProtocolDataPoster(CLProtocolDataPoster *pProtocolDataPoster);

private:
	CLDataPoster(const CLDataPoster&);
	CLDataPoster& operator=(const CLDataPoster&);

protected:
	CLProtocolDataPoster *m_pProtocolDataPoster;
};

#endif