#ifndef CLPostResultNotifier_H
#define CLPostResultNotifier_H

#include "CLStatus.h"

class CLPostResultNotifier
{
public:
	CLPostResultNotifier();
	virtual ~CLPostResultNotifier();

	virtual CLStatus Notify(bool bResult) = 0;

private:
	CLPostResultNotifier(const CLPostResultNotifier&);
	CLPostResultNotifier& operator=(const CLPostResultNotifier&);
};

#endif