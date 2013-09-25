#ifndef CLIteratorForIOVectors_H
#define CLIteratorForIOVectors_H

#include <list>
#include "CLStatus.h"

class CLIteratorForIOVectors
{
	friend class CLIOVectors;

public:
	CLIteratorForIOVectors();
	virtual ~CLIteratorForIOVectors();

	CLStatus Add(unsigned int steps);
	CLStatus Sub(unsigned int steps);

private:
	CLIteratorForIOVectors(const CLIteratorForIOVectors&);
	CLIteratorForIOVectors& operator=(const CLIteratorForIOVectors&);

private:
	char* m_pData;
	std::list<SLIOVectorItem>::iterator m_Iter;
	std::list<SLIOVectorItem> *m_pIOVectors;
};

#endif