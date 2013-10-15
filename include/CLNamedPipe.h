#ifndef CLNamedPipe_H
#define CLNamedPipe_H

#include <string>
#include "CLMutex.h"
#include "CLStatus.h"

class CLIOVectors;

class CLNamedPipe
{
public:
	CLNamedPipe(const char *pstrNamedPipe);
	virtual ~CLNamedPipe();

	int GetFd();
	long GetSizeForAtomWriting();

	CLStatus Read(CLIOVectors& IOVectors);
	CLStatus Write(CLIOVectors& IOVectors);

private:
	CLStatus UnsaftyRead(CLIOVectors& IOVectors);
	CLStatus UnsaftyWrite(CLIOVectors& IOVectors);

private:
	CLNamedPipe(const CLNamedPipe&);
	CLNamedPipe& operator=(const CLNamedPipe&);

private:
	std::string m_strNamedPipe;
	int m_Fd;
	long m_lSizeForAtomWriting;

	CLMutex m_Mutex;
};

#endif