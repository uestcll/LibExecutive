#ifndef CLMsgLoopManagerForIOMultiplexing_H
#define CLMsgLoopManagerForIOMultiplexing_H

#include <string>
#include "CLMessageLoopManager.h"

class CLMsgLoopManagerForIOMultiplexing
{
public:
	CLMsgLoopManagerForIOMultiplexing();
	virtual ~CLMsgLoopManagerForIOMultiplexing();

private:
	CLMsgLoopManagerForIOMultiplexing(const CLMsgLoopManagerForIOMultiplexing&);
	CLMsgLoopManagerForIOMultiplexing& operator=(const CLMsgLoopManagerForIOMultiplexing&);
};

#endif