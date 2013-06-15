#ifndef CLMutexByRecordLocking_H
#define CLMutexByRecordLocking_H

#include <string>
#include "CLMutexInterface.h"

using namespace std;

/************************************************************************/
/* 该类用于进程之间的互斥，但不能用于同一进程内线程间的互斥               */
/***********************************************************************/
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