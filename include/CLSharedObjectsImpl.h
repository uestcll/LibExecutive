#ifndef CLSharedObjectsImpl_H
#define CLSharedObjectsImpl_H

#include <string>
#include "DefinitionForConst.h"
#include "CLStatus.h"

using namespace std;

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