#ifndef CLExecutiveCommunication_H
#define CLExecutiveCommunication_H

#include "CLStatus.h"

class CLMessage;
class CLMessageSerializer;

class CLExecutiveCommunication
{
public:
	CLExecutiveCommunication(CLMessageSerializer *pMsgSerializer);
	virtual ~CLExecutiveCommunication();

	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage) = 0;

private:
	CLExecutiveCommunication(const CLExecutiveCommunication&);
	CLExecutiveCommunication& operator=(const CLExecutiveCommunication&);

private:
	CLMessageSerializer *m_pMsgSerializer;
};

#endif