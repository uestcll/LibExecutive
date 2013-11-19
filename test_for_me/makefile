main : ZTester.o CLMsgLoopManagerForSTLQueueTester.o CLMsgLoopManagerForPipeQueueTester.o test.o  ATester.o 
	g++ -I../../gtest-1.6.0/include -o main ZTester.o CLMsgLoopManagerForSTLQueueTester.o CLMsgLoopManagerForPipeQueueTester.o test.o ATester.o -L../ -lexecutive ../../gtest-1.6.0/libgtest.a -g -lpthread
	rm *.o

ATester.o : ATester.cpp
	g++ -I../../gtest-1.6.0/include -o ATester.o -c ATester.cpp -L../ -lexecutive -I../include -g

CLMsgLoopManagerForSTLQueueTester.o : CLMsgLoopManagerForSTLQueueTester.cpp
	g++ -I../../gtest-1.6.0/include -o CLMsgLoopManagerForSTLQueueTester.o -c CLMsgLoopManagerForSTLQueueTester.cpp -L../ -lexecutive -I../include -g

CLMsgLoopManagerForPipeQueueTester.o : CLMsgLoopManagerForPipeQueueTester.cpp
	g++ -I../../gtest-1.6.0/include -o CLMsgLoopManagerForPipeQueueTester.o -c CLMsgLoopManagerForPipeQueueTester.cpp -L../ -lexecutive -I../include -g


test.o : test.cpp
	g++ -I../../gtest-1.6.0/include -o test.o -c test.cpp -L../ -lexecutive -I../include -g

ZTester.o : ZTester.cpp
	g++ -I../../gtest-1.6.0/include -o ZTester.o -c ZTester.cpp -L../ -lexecutive -I../include -g


