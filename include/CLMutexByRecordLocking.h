#ifndef CLMutexByRecordLocking_H
#define CLMutexByRecordLocking_H

#include <string>
#include "CLMutexInterface.h"

using namespace std;

class CLMutexByRecordLocking : public CLMutexInterface
{
public:
	CLMutexByRecordLocking(const char *pstrFileName);
	virtual ~CLMutexByRecordLocking();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

private:
	CLMutexByRecordLocking(const CLMutexByRecordLocking&);
	CLMutexByRecordLocking& operator=(const CLMutexByRecordLocking&);

private:
	int m_Fd;
	string m_strFileName;
};

#endif