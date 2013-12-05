#ifndef CLTIMERAPPLYMSGDESERIALIZER_H
#define CLTIMERAPPLYMSGDESERIALIZER_H

#include "CLMessageDeserializer.h"

class CLTimerApplyMsgDeserializer : public CLMessageDeserializer
{
public:
	CLTimerApplyMsgDeserializer();
	virtual ~CLTimerApplyMsgDeserializer();

	virtual CLStatus Deserialize(CLIOVector &dataVec, CLMessage **pMsg);

private:
	CLTimerApplyMsgDeserializer(const CLTimerApplyMsgDeserializer&);
	CLTimerApplyMsgDeserializer& operator=(const CLTimerApplyMsgDeserializer&);
};

#endif