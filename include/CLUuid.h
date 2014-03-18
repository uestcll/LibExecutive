#ifndef CLUuid_H
#define CLUuid_H

#include <uuid/uuid.h>

class CLUuid
{
	friend class CLUuidComparer;

public:
	CLUuid();
	virtual ~CLUuid();

private:
	uuid_t m_Uuid;
};

class CLUuidComparer
{
public:
	int operator()(const CLUuid& u1, const CLUuid& u2) const;
};

#endif