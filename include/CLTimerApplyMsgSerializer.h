#ifndef CLTIMERAPPLYMSGSERIALIZER_H
#define CLTIMERAPPLYMSGSERIALIZER_H

#include "CLMessageSerializer.h"

class CLTimerApplyMsgSerializer : public CLMessageSerializer
{

public:
	CLTimerApplyMsgSerializer();
	virtual ~CLTimerApplyMsgSerializer();

	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVector *pDataVec);

private:
	CLTimerApplyMsgSerializer(const CLTimerApplyMsgSerializer&);
	CLTimerApplyMsgSerializer& operator=(const CLTimerApplyMsgSerializer&);

};

#endif