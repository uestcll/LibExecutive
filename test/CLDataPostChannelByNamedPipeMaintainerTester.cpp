#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLDataPostChannelByNamedPipeMaintainer, FunctionReturnRight)
{
	CLLogger::WriteLogMsg("CLDataPostChannelByNamedPipeMaintainer Test", 0);

	const char *strPath = "/tmp/NamedPipe_For_CLDataPostChannelByNamedPipeMaintainer_Test";
	CLNamedPipe pipe(strPath, true);

	CLDataPostChannelByNamedPipeMaintainer maintainer(strPath);

	CLInitialDataPostChannelNotifier *notifier = new CLInitialDataPostChannelNotifier();
	EXPECT_TRUE(maintainer.Initialize(notifier, 0).IsSuccess());

	CLDataPoster *poster = maintainer.GetDataPoster();
	CLDataPosterByNamedPipe *pq = dynamic_cast<CLDataPosterByNamedPipe *>(poster);
	EXPECT_TRUE(pq != 0);

	delete poster;
}