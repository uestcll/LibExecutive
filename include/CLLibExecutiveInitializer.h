#ifndef CLLibExecutiveInitializer_H
#define CLLibExecutiveInitializer_H

#include <pthread.h>
#include "CLStatus.h"

class CLLibExecutiveInitializer
{
public:
	static CLStatus Initialize();
	static CLStatus Destroy();

private:
	CLLibExecutiveInitializer();
	~CLLibExecutiveInitializer();
	CLLibExecutiveInitializer(const CLLibExecutiveInitializer&);
	CLLibExecutiveInitializer& operator=(const CLLibExecutiveInitializer&);

private:
	static bool m_bInitialized;
	static bool m_bDestroyed;

	static pthread_mutex_t m_MutexForInitializer;
};

#endif