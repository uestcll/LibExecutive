#ifndef CLCriticalSection_H
#define CLCriticalSection_H

class CLMutex;

class CLCriticalSection
{
public:
	/*
	出错时构造函数和析构函数将会抛出字符串类型异常
	*/
	CLCriticalSection(CLMutex *pMutex);
	virtual ~CLCriticalSection();

private:
	CLCriticalSection(const CLCriticalSection&);
	CLCriticalSection& operator=(const CLCriticalSection&);

private:
	CLMutex *m_pMutex;
};

#endif