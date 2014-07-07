#ifndef CLChannelErrorMsg_H
#define CLChannelErrorMsg_H

#include "CLMessage.h"

class CLChannelErrorMsg : public CLMessage
{
public:
	CLChannelErrorMsg(long errCode);
	virtual ~CLChannelErrorMsg();

public:
	const long& m_clErrorCode;

private:
	long m_lErrorCode;

private:
	CLChannelErrorMsg(const CLChannelErrorMsg&);
	CLChannelErrorMsg& operator=(const CLChannelErrorMsg&);
};

#endif