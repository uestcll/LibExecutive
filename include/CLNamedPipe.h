#ifndef CLNamedPipe_H
#define CLNamedPipe_H

#include <string>
#include "CLStatus.h"

class CLMutex;
class CLIOVectors;

class CLNamedPipe
{
public:
	explicit CLNamedPipe(const char *pstrNamedPipe);
	CLNamedPipe(const char* pstrNamedPipe, const char *pstrMutexName);
	virtual ~CLNamedPipe();

	int GetFd();
	long GetSizeForAtomWriting();

	CLStatus Read(CLIOVectors& IOVectors);
	CLStatus Write(CLIOVectors& IOVectors);

private:
	CLStatus InitialNamedPipe(const char *pstrNamedPipe);

	CLStatus UnsaftyRead(CLIOVectors& IOVectors);
	CLStatus UnsaftyWrite(CLIOVectors& IOVectors);

private:
	CLNamedPipe(const CLNamedPipe&);
	CLNamedPipe& operator=(const CLNamedPipe&);

private:
	std::string m_strNamedPipe;
	int m_Fd;
	long m_lSizeForAtomWriting;

	CLMutex *m_pMutex;
};

#endif