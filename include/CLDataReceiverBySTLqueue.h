#ifndef CLDataReceiverBySTLqueue_H
#define CLDataReceiverBySTLqueue_H

#include <queue>
#include "CLDataReceiver.h"
#include "CLMutex.h"

class CLDataReceiverBySTLqueue : public CLDataReceiver
{
public:
	CLDataReceiverBySTLqueue();
	virtual ~CLDataReceiverBySTLqueue();

	virtual CLStatus GetData(CLIOVectors *pIOVectors, void **ppContext);
	CLStatus PushData(unsigned long ulData);

private:
	CLDataReceiverBySTLqueue(const CLDataReceiverBySTLqueue&);
	CLDataReceiverBySTLqueue& operator=(const CLDataReceiverBySTLqueue&);

private:
	std::queue<unsigned long> m_DataQueue;
	CLMutex m_Mutex;
};

#endif