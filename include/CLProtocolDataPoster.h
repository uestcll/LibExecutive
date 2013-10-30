#ifndef CLPROTOCOLDATAPOSTER_H
#define CLPROTOCOLDATAPOSTER_H

#include "CLStatus.h"
#include <list>

class CLDataPoster;
class CLIOVector;
class CLEvent;

class CLProtocolDataPoster
{
public:
	CLProtocolDataPoster();
	virtual ~CLProtocolDataPoster();

public:
	CLStatus SetDataPoster(CLDataPoster *pDataPoster);
	CLStatus SetEventNotify(CLEvent *pEvent);
	CLStatus PostProtoData(CLIOVector *pDataVec);
	CLStatus PostData(CLIOVector *pDataVec);
	CLStatus PostLeftProtoData();

private:
	CLDataPoster *m_pDataPoster;
	std::list<CLIOVector *> m_MsgBufVecList;
	int m_topMsgBufIndex;
	bool m_bDataLeftFlag;
	CLEvent *m_pEvent;
};

#endif