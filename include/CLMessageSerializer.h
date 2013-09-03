#ifndef CLMessageSerializer_H
#define CLMessageSerializer_H

#include "CLStatus.h"

class CLMessage;
class CLIOVectors;

class CLMessageSerializer
{
public:
	CLMessageSerializer();
	virtual ~CLMessageSerializer();

	virtual CLStatus Serialize(CLMessage *pMsg, CLIOVectors *pIOVectors) = 0;

private:
	CLMessageSerializer(const CLMessageSerializer&);
	CLMessageSerializer& operator=(const CLMessageSerializer&);
};

#endif