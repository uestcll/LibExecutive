#ifndef CLNamedPipe_H
#define CLNamedPipe_H

#include <string>
#include "CLStatus.h"

class CLMutex;
class CLIOVectors;

class CLNamedPipe
{
public:
	CLNamedPipe(const char* pstrNamedPipe, bool bReader, const char *pstrMutexName);
	virtual ~CLNamedPipe();

	int GetFd();
	long GetSizeForAtomWriting();

	CLStatus Read(CLIOVectors& IOVectors);
	CLStatus Write(CLIOVectors& IOVectors);

private:
	CLStatus InitialReader();
	CLStatus InitialWriter();

	CLStatus UnsaftyRead(CLIOVectors& IOVectors);
	CLStatus UnsaftyWrite(CLIOVectors& IOVectors);

private:
	CLNamedPipe(const CLNamedPipe&);
	CLNamedPipe& operator=(const CLNamedPipe&);

private:
	std::string m_strNamedPipe;
	int m_Fd;
	long m_lSizeForAtomWriting;
	bool m_bReader;
	CLMutex *m_pMutex;
};

#endif