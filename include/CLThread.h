#ifndef CLTHREAD_H
#define CLTHREAD_H

#include <pthread.h>
#include "CLExecutive.h"
#include "CLStatus.h"
#include "CLEvent.h"

/*
CLThread代表了一个线程，该类对象的生存期，即线程的生存期

该类对象必须从堆中分配，且不必调用delete释放内存

构造函数中的bWaitForDeath用于指示是否需要等待新线程结束。默认
情况是不需要的。若需要则设置为true，并且需保证最后一定要调用
WaitForDeath函数，否则资源无法释放

Run、WaitForDeath函数如果返回成功，则只能调用一次。
Run方法调用失败，将导致CLThread对象被析构。因此，重试Run方法，只能创建一个新对象
*/
class CLThread : public CLExecutive
{
public:
	/*
	构造和析构函数出错时，会抛出字符串类型异常
	*pExecutiveFunctionProvider参数必须是从堆中分配的
	*/
	explicit CLThread(CLExecutiveFunctionProvider *pExecutiveFunctionProvider);
	CLThread(CLExecutiveFunctionProvider *pExecutiveFunctionProvider, bool bWaitForDeath);
	virtual ~CLThread();

	virtual CLStatus Run(void *pContext = 0);

	virtual CLStatus WaitForDeath();

private:
	static void* StartFunctionOfThread(void *pContext);

private:
	void *m_pContext;
	
	CLEvent m_EventForWaitingForNewThread;
	CLEvent m_EventForWaitingForOldThread;

	pthread_t m_ThreadID; 

	bool m_bWaitForDeath;
	bool m_bThreadCreated;
};

#endif
