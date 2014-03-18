#include <uuid/uuid.h>
#include "CLUuid.h"

CLUuid::CLUuid()
{
	uuid_generate(m_Uuid);
}

CLUuid::~CLUuid()
{
}

int CLUuidComparer::operator()(const CLUuid& u1, const CLUuid& u2) const
{
	int r = uuid_compare(u1.m_Uuid, u2.m_Uuid);
	if(r >= 0)
		return 0;
	else
		return 1;
}