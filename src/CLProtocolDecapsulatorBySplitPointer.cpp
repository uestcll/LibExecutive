#include "CLProtocolDecapsulatorBySplitPointer.h"
#include "CLIOVectors.h"

CLProtocolDecapsulatorBySplitPointer::CLProtocolDecapsulatorBySplitPointer() : CLProtocolDecapsulator(0)
{
}

CLProtocolDecapsulatorBySplitPointer::~CLProtocolDecapsulatorBySplitPointer()
{
}

CLStatus CLProtocolDecapsulatorBySplitPointer::Decapsulate(CLIOVectors& IOVectors, std::vector<CLIOVectors>& vSerializedMsgs, SLMessageScopeInIOVectors *pPartialMsgScope, void *pContext)
{
}