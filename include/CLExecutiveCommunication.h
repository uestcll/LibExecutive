#ifndef CLExecutiveCommunication_H
#define CLExecutiveCommunication_H

#include "CLStatus.h"

class CLMessage;

class CLExecutiveCommunication
{
public:
	CLExecutiveCommunication();
	virtual ~CLExecutiveCommunication();

	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage) = 0;

private:
	CLExecutiveCommunication(const CLExecutiveCommunication&);
	CLExecutiveCommunication& operator=(const CLExecutiveCommunication&);
};

#endif