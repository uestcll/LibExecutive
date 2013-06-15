#ifndef CLProcess_H
#define CLProcess_H

#include <unistd.h>
#include "CLExecutive.h"

class CLProcess : public CLExecutive
{
public:
	explicit CLProcess(CLExecutiveFunctionProvider *pExecutiveFunctionProvider);
	CLProcess(CLExecutiveFunctionProvider *pExecutiveFunctionProvider, bool bWaitForDeath);

	virtual ~CLProcess();

	virtual CLStatus Run(void *pstrCmdLine = 0);
	virtual CLStatus WaitForDeath();

private:
	CLStatus CloseFileDescriptor();
	void RunChildFunction(void *pstrCmdLine);

private:
	CLProcess(const CLProcess&);
	CLProcess& operator=(const CLProcess&);

	pid_t m_ProcessID;

	bool m_bProcessCreated;
	bool m_bWaitForDeath;
	bool m_bExecSuccess;
};

#endif