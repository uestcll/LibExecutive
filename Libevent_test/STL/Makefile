test : CLMsgLoopManagerForSTLQueue_test.o
	g++ -I ../include -L .. -o test CLMsgLoopManagerForSTLQueue_test.o  -g -lexecutive -lpthread -luuid
	rm *.o

CLMsgLoopManagerForSTLQueue_test.o : ./CLMsgLoopManagerForSTLQueue_test.cpp
	g++ -I ../include -o CLMsgLoopManagerForSTLQueue_test.o -c CLMsgLoopManagerForSTLQueue_test.cpp -g
