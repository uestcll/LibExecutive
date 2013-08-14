main : test.o ATester.o CLStatusTester.o CLLoggerTester.o CLSharedMemoryTester.o CLSharedMutexAllocatorTester.o CLMutexTester.o CLSharedConditionVariableAllocatorTester.o CLConditionVariableTester.o CLSharedEventAllocatorTester.o CLEventTester.o CLCriticalSectionTester.o ZTester.o
	g++ -o main test.o ATester.o CLStatusTester.o CLLoggerTester.o CLSharedMemoryTester.o CLSharedMutexAllocatorTester.o CLMutexTester.o CLSharedConditionVariableAllocatorTester.o CLConditionVariableTester.o CLSharedEventAllocatorTester.o CLEventTester.o CLCriticalSectionTester.o ZTester.o ../libexecutive.a ../lib/gtest-1.4.0/libgtest.a -g -lpthread
	rm *.o

test.o : test.cpp
	g++ -I../lib/gtest-1.4.0/include -o test.o -c test.cpp -I../include -g

ATester.o : ATester.cpp
	g++ -I../lib/gtest-1.4.0/include -o ATester.o -c ATester.cpp -I../include -g

CLStatusTester.o : CLStatusTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLStatusTester.o -c CLStatusTester.cpp -I../include -g

CLLoggerTester.o : CLLoggerTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLLoggerTester.o -c CLLoggerTester.cpp -I../include -g

CLSharedMemoryTester.o : CLSharedMemoryTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLSharedMemoryTester.o -c CLSharedMemoryTester.cpp -I../include -g

CLSharedMutexAllocatorTester.o : CLSharedMutexAllocatorTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLSharedMutexAllocatorTester.o -c CLSharedMutexAllocatorTester.cpp -I../include -g

CLMutexTester.o : CLMutexTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLMutexTester.o -c CLMutexTester.cpp -I../include -g

CLSharedConditionVariableAllocatorTester.o : CLSharedConditionVariableAllocatorTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLSharedConditionVariableAllocatorTester.o -c CLSharedConditionVariableAllocatorTester.cpp -I../include -g

CLConditionVariableTester.o : CLConditionVariableTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLConditionVariableTester.o -c CLConditionVariableTester.cpp -I../include -g

CLSharedEventAllocatorTester.o : CLSharedEventAllocatorTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLSharedEventAllocatorTester.o -c CLSharedEventAllocatorTester.cpp -I../include -g

CLEventTester.o : CLEventTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLEventTester.o -c CLEventTester.cpp -I../include -g

CLCriticalSectionTester.o : CLCriticalSectionTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o CLCriticalSectionTester.o -c CLCriticalSectionTester.cpp -I../include -g

ZTester.o : ZTester.cpp
	g++ -I../lib/gtest-1.4.0/include -o ZTester.o -c ZTester.cpp -I../include -g