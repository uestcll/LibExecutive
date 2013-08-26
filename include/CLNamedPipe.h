#ifndef CLNamedPipe_H
#define CLNamedPipe_H

#include <string>
#include "CLStatus.h"

class CLNamedPipe
{
public:
	CLNamedPipe(const char *pstrNamedPipe);
	virtual ~CLNamedPipe();

	int GetFd();
	long GetSizeForAtomWriting();

	CLStatus Read(char *pBuf, unsigned long ulBufSize);
	CLStatus Write(char *pBuf, unsigned long ulBufSize);

private:
	CLNamedPipe(const CLNamedPipe&);
	CLNamedPipe& operator=(const CLNamedPipe&);

private:
	std::string m_strNamedPipe;
	int m_Fd;
	long m_lSizeForAtomWriting;
};

#endif