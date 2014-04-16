#include "CLMessageReceiverByTCPListener.h"
#include "CLDataReceiverByTCPAccept.h"
#include "CLPointerMsgDeserializer.h"
#include "CLProtoParserForPointerMsg.h"

CLMessageReceiverByTCPListener::CLMessageReceiverByTCPListener(
	const char *pHostNameOrIp, 
	const char *pServiceNameOrPort, 
	bool isBlock, 
	int listenNum) : 
	CLMessageReceiver(
		new CLDataReceiverByTCPAccept(
			pHostNameOrIp, pServiceNameOrPort, isBlock, listenNum), 
		new CLProtoParserForPointerMsg, new CLPointerMsgDeserializer)
{

}

