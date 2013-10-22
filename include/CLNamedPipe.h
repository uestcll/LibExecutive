#ifndef CLNAMEDPIPE_H
#define CLNAMEDPIPE_H

#include <string>
#include "CLStatus.h"

#define PIPE_FOR_READ 20
#define PIPE_FOR_WRITE 30
#define FILE_PATH_FOR_NAMED_PIPE "/tmp/"

using namespace std;

class CLIOVector;

class CLNamedPipe
{
public:
	CLNamedPipe(const char* pStrPipeName, int flag);
	virtual ~CLNamedPipe();

	long GetAtomWriteSize();

	CLStatus Read(char *pBuf, int length);
	CLStatus Write(char *pBuf, int length);

	CLStatus ReadVecs(CLIOVector dataVec);
	CLStatus WriteVecs(CLIOVector dataVec);
private:
	string m_strPipeName;
	long m_lAtomWriteSize;
	int m_Fd;
};

#endif