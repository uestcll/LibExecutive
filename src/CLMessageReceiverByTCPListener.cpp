#include "CLMessageReceiverByTCPListener.h"
#include "CLDataReceiverByTCPAccept.h"
#include "CLPointerMsgDeserializer.h"
#include "CLProtoParserForPointerMsg.h"
#include "CLBuffer.h"

CLMessageReceiverByTCPListener::CLMessageReceiverByTCPListener(
	const char *pHostNameOrIp, 
	const char *pServiceNameOrPort,
	bool isBlock,
	int listenNum) : 
	CLMessageReceiver(new CLDataReceiverByTCPAccept(
		pHostNameOrIp, pServiceNameOrPort, isBlock, listenNum), 
		new CLProtoParserForPointerMsg, 
		new CLPointerMsgDeserializer)
{

}

// CLMessageReceiverByTCPListener::CLMessageReceiverByTCPListener(
// 	const char *pHostNameOrIp,
// 	const char *pServiceNameOrPort,
// 	bool isBlock,
// 	int listenNum)
// {
// 	m_pDataReceiver = new CLDataReceiverByTCPAccept(pHostNameOrIp, pServiceNameOrPort, isBlock, listenNum);
// 	m_pProtoParser = new CLProtoParserForPointerMsg;
// 	m_pMsgDeserializer = new CLPointerMsgDeserializer;

// 	m_pDataBuffer = new CLBuffer;
// }

CLMessageReceiverByTCPListener::~CLMessageReceiverByTCPListener()
{

}
