#ifndef CLIteratorForIOVectors_H
#define CLIteratorForIOVectors_H

#include <list>

struct SLIOVectorItem;

class CLIteratorForIOVectors
{
	friend class CLIOVectors;

public:
	CLIteratorForIOVectors();
	virtual ~CLIteratorForIOVectors();

private:
	CLIteratorForIOVectors(const CLIteratorForIOVectors&);
	CLIteratorForIOVectors& operator=(const CLIteratorForIOVectors&);

private:
	char* m_pData;
	std::list<SLIOVectorItem>::iterator m_Iter;
};

#endif