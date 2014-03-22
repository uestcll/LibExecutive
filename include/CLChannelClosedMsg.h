#ifndef CLChannelClosedMsg_H
#define CLChannelClosedMsg_H

#include "CLMessage.h"

class CLChannelClosedMsg : public CLMessage
{
public:
	CLChannelClosedMsg();
	virtual ~CLChannelClosedMsg();

private:
	CLChannelClosedMsg(const CLChannelClosedMsg&);
	CLChannelClosedMsg& operator=(const CLChannelClosedMsg&);
};

#endif