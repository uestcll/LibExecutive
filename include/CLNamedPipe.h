#ifndef CLNAMEDPIPE_H
#define CLNAMEDPIPE_H

#include <string>
#include "CLStatus.h"
#include "CLMutex.h"

#define PIPE_FOR_READ 20
#define PIPE_FOR_WRITE 30
#define FILE_PATH_FOR_NAMED_PIPE "/tmp/"

using namespace std;

class CLIOVector;

class CLNamedPipe
{
public:
	CLNamedPipe(const char* pStrPipeName, bool isShared, int flag);
	// CLNamedPipe(const char* pStrPipeName, const char* pstrMutexName, int flag);
	virtual ~CLNamedPipe();

	long GetAtomWriteSize();

	const int GetPipeFd() const;
	CLStatus Read(char *pBuf, int length);
	CLStatus Write(char *pBuf, int length);

	CLStatus ReadVecs(CLIOVector& dataVec);
	CLStatus WriteVecs(CLIOVector& dataVec);

private:
	CLStatus Initialize(const char* pStrPipeName, int flag);

private:

	string m_strPipeName;
	long m_lAtomWriteSize;
	int m_Fd;
	CLMutex *m_pMutexForNamedPipe;
};

#endif