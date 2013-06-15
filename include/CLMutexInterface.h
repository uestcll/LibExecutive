#ifndef CLMutexInterface_H
#define CLMutexInterface_H

#include "CLStatus.h"

class CLMutexInterface
{
public:
	CLMutexInterface();
	virtual ~CLMutexInterface();

	virtual CLStatus Initialize() = 0;
	virtual CLStatus Uninitialize() = 0;

	virtual CLStatus Lock() = 0;
	virtual CLStatus Unlock() = 0;

private:
	CLMutexInterface(const CLMutexInterface&);
	CLMutexInterface& operator=(const CLMutexInterface&);
};

#endif