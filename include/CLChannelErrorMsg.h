#ifndef CLChannelErrorMsg_H
#define CLChannelErrorMsg_H

#include "CLMessage.h"

class CLChannelErrorMsg : public CLMessage
{
public:
	CLChannelErrorMsg();
	CLChannelErrorMsg(long errCode);
	virtual ~CLChannelErrorMsg();

public:
	long m_lErrCode;

private:
	CLChannelErrorMsg(const CLChannelErrorMsg&);
	CLChannelErrorMsg& operator=(const CLChannelErrorMsg&);
};

#endif