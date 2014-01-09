#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLDataPostChannelBySTLqueueMaintainer, FunctionReturnRight)
{
	CLLogger::WriteLogMsg("CLDataPostChannelBySTLqueueMaintainer Test", 0);

	CLSTLqueue *q = new CLSTLqueue();
	CLDataPostChannelBySTLqueueMaintainer maintainer(q);

	CLInitialDataPostChannelNotifier *notifier = new CLInitialDataPostChannelNotifier();
	EXPECT_TRUE(maintainer.Initialize(notifier, 0).IsSuccess());

	CLDataPoster *poster = maintainer.GetDataPoster();
	CLDataPosterBySTLqueue *pq = dynamic_cast<CLDataPosterBySTLqueue *>(poster);
	EXPECT_TRUE(pq != 0);

	delete poster;
}