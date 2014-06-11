#ifndef CLGetMessageErrorMsg_H
#define CLGetMessageErrorMsg_H

#include "CLMessage.h"

class CLGetMessageErrorMsg : public CLMessage
{
public:
	CLGetMessageErrorMsg();
	virtual ~CLGetMessageErrorMsg();

private:
	CLGetMessageErrorMsg(const CLGetMessageErrorMsg&);
	CLGetMessageErrorMsg& operator=(const CLGetMessageErrorMsg&);
};

#endif