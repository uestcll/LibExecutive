#ifndef CLNAMEDPIPE_H
#define CLNAMEDPIPE_H

#include <string>
#include "CLStatus.h"

#define PIPE_FOR_READ 20
#define PIPE_FOR_WRITE 30
#define FILE_PATH_FOR_NAMED_PIPE "/tmp/"

using namespace std;

class CLNamedPipe
{
public:
	CLNamedPipe(const char* pStrPipeName, int flag);
	virtual ~CLNamedPipe();

	CLStatus Read(char *pBuf, int length);
	CLStatus Write(char *pBuf, int length);

private:
	string m_strPipeName;
	int m_Fd;
};

#endif