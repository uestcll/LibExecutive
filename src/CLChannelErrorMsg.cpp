#include "CLChannelErrorMsg.h"
#include "DefinitionForConst.h"
#include "ErrorCode.h"

CLChannelErrorMsg::CLChannelErrorMsg(long errCode) : CLMessage(MESSAGE_ID_FOR_CHANNEL_ERROR), m_clErrorCode(m_lErrorCode)
{	
	m_lErrorCode = errCode;
}

CLChannelErrorMsg::~CLChannelErrorMsg()
{
}