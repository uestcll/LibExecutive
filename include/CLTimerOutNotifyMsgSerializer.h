#ifndef CLTIMEROUTNOTIFYMSGSERIALIZER_H
#define CLTIMEROUTNOTIFYMSGSERIALIZER_H

#include "CLMessageSerializer.h"

class CLTimerOutNotifyMsgSerializer : public CLMessageSerializer
{
public:
	CLTimerOutNotifyMsgSerializer();
	virtual ~CLTimerOutNotifyMsgSerializer();

public:
	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec);

private:
	CLTimerOutNotifyMsgSerializer(const CLTimerOutNotifyMsgSerializer&);
	CLTimerOutNotifyMsgSerializer& operator=(const CLTimerOutNotifyMsgSerializer&);
};


#endif