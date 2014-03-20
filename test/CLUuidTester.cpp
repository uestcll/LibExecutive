#include <gtest/gtest.h>
#include <map>
#include "LibExecutive.h"

using namespace std;

TEST(CLUuid, FeatureTest)
{
	CLLogger::WriteLogMsg("CLUuid Test", 0);

	const int count = 1000000;

	map<CLUuid, int, CLUuidComparer> table;
	CLUuid *ppUuid = new CLUuid[count];

	for(int i = 0; i < count; i++)
	{
		map<CLUuid, int, CLUuidComparer>::iterator it = table.find(ppUuid[i]);
		if(it != table.end())
		{
			EXPECT_TRUE(false);
		}

		table[ppUuid[i]] = i;
	}

	for(int i = 0; i < count; i++)
	{
		map<CLUuid, int, CLUuidComparer>::iterator it = table.find(ppUuid[i]);
		EXPECT_TRUE(it->second == i);
	}

	delete [] ppUuid;
}