#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <syscall.h>
#include "CLProcess.h"
#include "CLExecutiveFunctionProvider.h"
#include "CLLogger.h"

using namespace std;

#define LENGTH_OF_PROCESSID 25
#define LENGTH_OF_PATH 1025

CLProcess::CLProcess(CLExecutiveFunctionProvider *pExecutiveFunctionProvider) : CLExecutive(pExecutiveFunctionProvider)
{
	m_bProcessCreated = false;
	m_bWaitForDeath = false;
	m_bExecSuccess = true;
	m_ProcessID = -1;
}

CLProcess::CLProcess(CLExecutiveFunctionProvider *pExecutiveFunctionProvider, bool bWaitForDeath) : CLExecutive(pExecutiveFunctionProvider)
{
	m_bProcessCreated = false;
	m_bWaitForDeath = bWaitForDeath;
	m_bExecSuccess = true;
	m_ProcessID = -1;
}

CLProcess::~CLProcess()
{
}

void CLProcess::RunChildFunction(void *pstrCmdLine)
{
	m_ProcessID = syscall(SYS_getpid);

	CloseFileDescriptor();

	m_pExecutiveFunctionProvider->RunExecutiveFunction(pstrCmdLine);

	m_bExecSuccess = false;

	_exit(0);
}

CLStatus CLProcess::Run(void *pstrCmdLine)
{
	if(m_bProcessCreated)
		return CLStatus(-1, 0);

	pid_t pid = vfork();
	if(pid == -1)
	{
		CLLogger::WriteLogMsg("In CLProcess::Run(), vfork error", errno);
		delete this;
		return CLStatus(-1, 0);
	}

	if(pid == 0)
	{
		if(m_bWaitForDeath)
			RunChildFunction(pstrCmdLine);
		else
		{
			pid_t pid1 = vfork();
			if(pid1 == -1)
			{
				CLLogger::WriteLogMsg("In CLProcess::Run(), vfork2 error", errno);
				m_bExecSuccess = false;
				_exit(0);
			}

			if(pid1 == 0)
				RunChildFunction(pstrCmdLine);

			_exit(0);
		}
	}

	if(!m_bWaitForDeath)
		if(waitpid(pid, 0, 0) == -1)
			CLLogger::WriteLogMsg("In CLProcess::Run(), waitpid error", errno);

	if(!m_bExecSuccess)
	{
		delete this;
		return CLStatus(-1, 0);
	}

	m_bProcessCreated = true;

	if(!m_bWaitForDeath)
		delete this;

	return CLStatus(0, 0);
}

CLStatus CLProcess::WaitForDeath()
{
	if(!m_bWaitForDeath)
		return CLStatus(-1, 0);

	if(!m_bProcessCreated)
		return CLStatus(-1, 0);

	if(waitpid(m_ProcessID, 0, 0) == -1)
	{
		CLLogger::WriteLogMsg("In CLProcess::WaitForDeath(), waitpid error", errno);
		return CLStatus(-1, errno);
	}

	delete this;

	return CLStatus(0, 0);
}

CLStatus CLProcess::CloseFileDescriptor()
{
	string strPath = "/proc/";

	char id[LENGTH_OF_PROCESSID];
	snprintf(id, LENGTH_OF_PROCESSID, "%d", m_ProcessID);

	strPath += id;
	strPath += "/fd";

	string strPath1 = strPath;

	strPath += "/";

	DIR *pDir = opendir(strPath.c_str());
	if(pDir == 0)
	{
		CLLogger::WriteLogMsg("In CLProcess::CloseFileDescriptor(), opendir error", 0);
		return CLStatus(-1, 0);
	}

	while(struct dirent *pDirent = readdir(pDir))
	{
		char captial = pDirent->d_name[0];
		if((captial == '.') || (captial == '0') || (captial == '1') || (captial == '2'))
			continue;

		int fd = atoi(pDirent->d_name);
		if(fd != 0)
		{
			string strTmpPath = strPath;
			strTmpPath += pDirent->d_name;

			char pathname[LENGTH_OF_PATH] = {0};
			if(readlink(strTmpPath.c_str(), pathname, LENGTH_OF_PATH) == -1)
			{
				CLLogger::WriteLogDirectly("In CLProcess::CloseFileDescriptor(), readlink error", errno);
				continue;
			}

			if(strcmp(pathname, strPath1.c_str()) == 0)
				continue;

			if(close(fd) == -1)
			{
				string errormsg = "In CLProcess::CloseFileDescriptor(), close error, file: ";
				errormsg += pDirent->d_name;
				CLLogger::WriteLogDirectly(errormsg.c_str(), errno);
			}
		}
	}

	if(closedir(pDir) == -1)
	{
		CLLogger::WriteLogDirectly("In CLProcess::CloseFileDescriptor(), closedir error", errno);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}