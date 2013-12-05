#ifndef CLTIMTEROUTNOTIFYMSGDESERIALIZER_H
#define CLTIMTEROUTNOTIFYMSGDESERIALIZER_H

#include "CLMessageDeserializer.h"

class CLTimerOutNotifyMsgDeserializer : public CLMessageDeserializer
{
public:
	CLTimerOutNotifyMsgDeserializer();
	virtual ~CLTimerOutNotifyMsgDeserializer();

	virtual CLStatus Deserialize(CLIOVector &dataVec, CLMessage **pMsg);

private:
	CLTimerOutNotifyMsgDeserializer(const CLTimerOutNotifyMsgDeserializer&);
	CLTimerOutNotifyMsgDeserializer& operator=(const CLTimerOutNotifyMsgDeserializer&);
};

#endif