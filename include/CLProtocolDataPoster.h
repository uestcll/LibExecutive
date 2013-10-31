#ifndef CLPROTOCOLDATAPOSTER_H
#define CLPROTOCOLDATAPOSTER_H

#include "CLStatus.h"
#include <list>

#define POST_DATA_COMPLETE 201
#define POST_DATA_PARTION 202
#define POST_DATA_ERROR 203


class CLDataPoster;
class CLIOVector;
class CLEvent;

class CLProtocolDataPoster //deal with the msgDataVec buffer free and delete
{
public:
	CLProtocolDataPoster();
	virtual ~CLProtocolDataPoster();

public:
	CLStatus SetParameters(CLDataPoster *pDataPoster, CLEvent *pEvent);
	CLStatus PostProtoData(CLIOVector *pDataVec);
	CLStatus PostLeftProtoData();

private:
	CLStatus PostData(CLIOVector *pDataVec);

private:
	CLDataPoster *m_pDataPoster;
	std::list<CLIOVector *> m_MsgBufVecList;
	int m_topMsgBufIndex;
	bool m_bDataLeftFlag;
	CLEvent *m_pEvent; //delete is in the msgposter
};

#endif