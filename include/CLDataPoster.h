#ifndef CLDATAPOSTER_H
#define CLDATAPOSTER_H

#include "CLStatus.h"

// class CLDataChannelContainer;
class CLIOVector;
class CLProtocolDataPoster;
/*
//do not deal with the free of data vec
*/
class CLDataPoster
{
public:
	CLDataPoster();
	virtual ~CLDataPoster();

	virtual CLStatus PostData(CLIOVector* pDataVec) = 0;
	CLStatus SetProtocolDataPoster(CLProtocolDataPoster *pProtoDataPoster);

	// CLStatus SetChannelContainer(CLDataChannelContainer *pContainer);
	// CLStatus RecyclePoster();

protected:
	// CLDataChannelContainer *m_pContainer;
	// CLIOVector *m_pDataVecter;
	CLProtocolDataPoster *m_pProtoDataPoster;

private:
	CLDataPoster(const CLDataPoster&);
	CLDataPoster& operator=(const CLDataPoster&);
};

#endif