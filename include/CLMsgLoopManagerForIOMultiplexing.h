#ifndef CLMsgLoopManagerForIOMultiplexing_H
#define CLMsgLoopManagerForIOMultiplexing_H

#include <string>
#include <map>
#include "CLMessageLoopManager.h"

class CLMsgLoopManagerForIOMultiplexing : public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForIOMultiplexing(CLMessageObserver *pMsgObserver, const char* pstrThreadName);
	virtual ~CLMsgLoopManagerForIOMultiplexing();

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus WaitForMessage();

private:
	CLMsgLoopManagerForIOMultiplexing(const CLMsgLoopManagerForIOMultiplexing&);
	CLMsgLoopManagerForIOMultiplexing& operator=(const CLMsgLoopManagerForIOMultiplexing&);

private:
	std::string m_strThreadName;
	struct fd_set *m_pReadSet;
	struct fd_set *m_pWriteSet;
	//std::map<int
};

#endif