#ifndef CLPrivateExecutiveCommunicationByNamedPipe_H
#define CLPrivateExecutiveCommunicationByNamedPipe_H

#include "CLExecutiveCommunication.h"

class CLPrivateExecutiveCommunicationByNamedPipe : public CLExecutiveCommunication
{
public:
	CLPrivateExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName);
	virtual ~CLPrivateExecutiveCommunicationByNamedPipe();

	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage) = 0;

private:
	CLPrivateExecutiveCommunicationByNamedPipe(const CLPrivateExecutiveCommunicationByNamedPipe&);
	CLPrivateExecutiveCommunicationByNamedPipe& operator=(const CLPrivateExecutiveCommunicationByNamedPipe&);
};

#endif