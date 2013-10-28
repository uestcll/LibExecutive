#ifndef CLDATAPOSTER_H
#define CLDATAPOSTER_H

#include "CLStatus.h"

#define POST_DATA_COMPLETE 201
#define POST_DATA_PARTION 202
#define POST_DATA_ERROR 203
#define POST_DATA_BUF_ERROR 204

// class CLDataChannelContainer;
class CLIOVector;

class CLDataPoster
{
public:
	CLDataPoster();
	virtual ~CLDataPoster();

	virtual CLStatus PostData(CLIOVector& dataVec) = 0;

	// CLStatus SetChannelContainer(CLDataChannelContainer *pContainer);
	// CLStatus RecyclePoster();

protected:
	// CLDataChannelContainer *m_pContainer;
	// CLIOVector *m_pDataVecter;

private:
	CLDataPoster(const CLDataPoster&);
	CLDataPoster& operator=(const CLDataPoster&);
};

#endif