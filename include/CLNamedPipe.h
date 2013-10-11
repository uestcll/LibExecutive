#ifndef CLNAMEDPIPE_H
#define CLNAMEDPIPE_H

#include <string>
#include "CLStatus.h"


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