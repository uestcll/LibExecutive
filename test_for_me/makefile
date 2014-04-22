main : ZTester.o CLSocketTester.o  test.o CLMsgLoopManagerForSTLQueueTester.o CLMsgLoopManagerForPipeQueueTester.o ATester.o 
	g++ -I../../../../gtest-1.6.0/include -o main ZTester.o CLSocketTester.o test.o CLMsgLoopManagerForSTLQueueTester.o CLMsgLoopManagerForPipeQueueTester.o ATester.o -L../../../../gtest-1.6.0/lib/ -lgtest  -L../  -lexecutive  -g -lpthread
	rm *.o

ATester.o : ATester.cpp
	g++ -I../../../../gtest-1.6.0/include -o ATester.o -c ATester.cpp -L../ -lexecutive -I../include -g

CLSocketTester.o : CLSocketTester.cpp
	g++ -I../../../../gtest-1.6.0/include -o CLSocketTester.o -c CLSocketTester.cpp -L../ -lexecutive -I../include -g

CLMsgLoopManagerForSTLQueueTester.o : CLMsgLoopManagerForSTLQueueTester.cpp
	g++ -I../../../../gtest-1.6.0/include -o CLMsgLoopManagerForSTLQueueTester.o -c CLMsgLoopManagerForSTLQueueTester.cpp -L../ -lexecutive -I../include -g

CLMsgLoopManagerForPipeQueueTester.o : CLMsgLoopManagerForPipeQueueTester.cpp
	g++ -I../../../../gtest-1.6.0/include -o CLMsgLoopManagerForPipeQueueTester.o -c CLMsgLoopManagerForPipeQueueTester.cpp -L../ -lexecutive -I../include -g


test.o : test.cpp
	g++ -I../../../../gtest-1.6.0/include -o test.o -c test.cpp -L../ -lexecutive -I../include -g

ZTester.o : ZTester.cpp
	g++ -I../../../../gtest-1.6.0/include -o ZTester.o -c ZTester.cpp -L../ -lexecutive -I../include -g


