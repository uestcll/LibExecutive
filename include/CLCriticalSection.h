#ifndef CLCriticalSection_H
#define CLCriticalSection_H

class CLMutex;

class CLCriticalSection
{
public:
	CLCriticalSection(CLMutex *pMutex);
	virtual ~CLCriticalSection();

private:
	CLCriticalSection(const CLCriticalSection&);
	CLCriticalSection& operator=(const CLCriticalSection&);

private:
	CLMutex *m_pMutex;
};

#endif