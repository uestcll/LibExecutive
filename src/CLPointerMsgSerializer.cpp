#include "CLPointerMsgSerializer.h"
#include "CLIOVector.h"

CLPointerMsgSerializer::CLPointerMsgSerializer()
{

}

CLPointerMsgSerializer::~CLPointerMsgSerializer()
{

}

CLStatus CLPointerMsgSerializer::Serialize(CLMessage *pMsg, CLIOVector *pDataVec)
{
	if((pMsg == 0) || (pDataVec == 0))
    {
        CLLogger::WriteLogMsg("In CLPointerMsgSerializer::Serialize(), parameters error", 0);
        return CLStatus(-1, 0);
    }

    char *p = new char[sizeof(CLMessage *)];
    *((unsigned long *)p) = (unsigned long)pMsg; //where to delete msg

    pDataVec->PushBack(p, sizeof(CLMessage *));
    
    return CLStatus(0, 0);
}
