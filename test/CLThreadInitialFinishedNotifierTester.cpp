#include <gtest/gtest.h>
#include "LibExecutive.h"

TEST(CLThreadInitialFinishedNotifier, NotifyInitialFinished_Parameters_Test)
{
	CLThreadInitialFinishedNotifier notifier(0);
	EXPECT_FALSE(notifier.IsInitialSuccess());

	EXPECT_FALSE(notifier.NotifyInitialFinished(true).IsSuccess());
	EXPECT_TRUE(notifier.IsInitialSuccess());
}

TEST(CLThreadInitialFinishedNotifier, NotifyInitialFinished_Features_Test)
{
	CLEvent event;
	CLThreadInitialFinishedNotifier notifier(&event);

	EXPECT_TRUE(notifier.NotifyInitialFinished(true).IsSuccess());
	EXPECT_TRUE(event.Wait().IsSuccess());
	EXPECT_TRUE(notifier.IsInitialSuccess());

	EXPECT_TRUE(notifier.NotifyInitialFinished(false).IsSuccess());
	EXPECT_TRUE(event.Wait().IsSuccess());
	EXPECT_FALSE(notifier.IsInitialSuccess());
}