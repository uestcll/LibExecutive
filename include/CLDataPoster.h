#ifndef CLDataPoster_H
#define CLDataPoster_H

#include "CLStatus.h"

class CLIOVectors;
class CLProtocolDataPoster;

class CLDataPoster
{
public:
	CLDataPoster();
	virtual ~CLDataPoster();

	virtual CLStatus PostData(CLIOVectors *pIOVectors) = 0;
	
	virtual CLStatus PostDelayedData(CLIOVectors *pIOVectors);

	void SetProtocolDataPoster(CLProtocolDataPoster *pProtocolDataPoster);

private:
	CLDataPoster(const CLDataPoster&);
	CLDataPoster& operator=(const CLDataPoster&);

protected:
	CLProtocolDataPoster *m_pProtocolDataPoster;
};

#endif