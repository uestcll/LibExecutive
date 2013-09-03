#ifndef CLMsgToPointerSerializer_H
#define CLMsgToPointerSerializer_H

#include "CLMessageSerializer.h"

class CLMsgToPointerSerializer : public CLMessageSerializer
{
public:
	CLMsgToPointerSerializer();
	virtual ~CLMsgToPointerSerializer();

	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors);

private:
	CLMsgToPointerSerializer(const CLMsgToPointerSerializer&);
	CLMsgToPointerSerializer& operator=(const CLMsgToPointerSerializer&);
};

#endif