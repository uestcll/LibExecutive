#include "CLChannelErrorMsg.h"
#include "DefinitionForConst.h"
#include "ErrorCode.h"

CLChannelErrorMsg::CLChannelErrorMsg() : CLMessage(MESSAGE_ID_FOR_CHANNEL_ERROR), m_lErrCode(NORMAL_ERROR)
{
}

CLChannelErrorMsg::CLChannelErrorMsg(long errCode) : CLMessage(MESSAGE_ID_FOR_CHANNEL_ERROR), m_lErrCode(errCode)
{	
}

CLChannelErrorMsg::~CLChannelErrorMsg()
{
}