#ifndef CLSharedMemory_H
#define CLSharedMemory_H

#include "CLStatus.h"
#include "CLMutex.h"

class CLSharedMemory
{
public:
	explicit CLSharedMemory(const char *pstrFileName, size_t nSize = 0);
	virtual ~CLSharedMemory();

	void *GetAddress();
	int GetRefCount();

private:
	CLStatus DeleteSharedMemory();

private:
	CLSharedMemory(const CLSharedMemory&);
	CLSharedMemory& operator=(const CLSharedMemory&);

private:
	void *m_pSharedMemory;
	int m_SharedMemoryID;

	CLMutex m_Mutex;
};

#endif