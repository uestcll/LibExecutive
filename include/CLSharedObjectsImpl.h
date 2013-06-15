#ifndef CLSharedObjectsImpl_H
#define CLSharedObjectsImpl_H

#include <string>
#include "CLStatus.h"

using namespace std;

#define LENGTH_OF_SHARED_OBJECT_NAME 256

#define UNINITIALIZED_SHARED_OBJECT 0
#define INITIALIZED_SHARED_OBJECT 1
#define ALLOCATED_SHARED_OBJECT 2

#define NUMBER_OF_SHARED_OBJECT 1024
#define MAGIC_NUMBER_FOR_SHARED_OBJECT 0x12345678

class CLSharedMemory;

struct SLSharedObjectHead
{
	int Status;
	int RefCount;
	char strSharedObjectName[LENGTH_OF_SHARED_OBJECT_NAME];
};

class CLSharedObjectsImpl
{
public:
	CLSharedObjectsImpl();
	virtual ~CLSharedObjectsImpl();

	CLStatus Initialize();
	CLStatus Destroy();

	void *GetSharedObject(const char *pstrSharedObjectName);
	CLStatus ReleaseSharedObject(const char *pstrSharedObjectName);

	virtual CLStatus InitializeSharedObject(SLSharedObjectHead *pObject) = 0;
	virtual CLStatus DestroySharedObject(SLSharedObjectHead *pObject) = 0;

private:
	CLSharedObjectsImpl(const CLSharedObjectsImpl&);
	CLSharedObjectsImpl& operator=(const CLSharedObjectsImpl&);

protected:
	CLSharedMemory *m_pSharedMemory;
	unsigned int m_nItemSize;
	string m_strSharedSpaceName;
	SLSharedObjectHead *m_pSharedObjects;
};

#endif