#ifndef CLDataPostResultNotifier_H
#define CLDataPostResultNotifier_H

#include "CLStatus.h"

class CLDataPostResultNotifier
{
public:
	CLDataPostResultNotifier();
	virtual ~CLDataPostResultNotifier();

	virtual CLStatus Notify(int Result) = 0;

private:
	CLDataPostResultNotifier(const CLDataPostResultNotifier&);
	CLDataPostResultNotifier& operator=(const CLDataPostResultNotifier&);
};

#endif