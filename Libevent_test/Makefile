test : CLMsgLoopManagerForLibevent_test.o
	g++ -I ../include -L .. -o test CLMsgLoopManagerForLibevent_test.o  -g -lexecutive -lpthread -luuid
	rm *.o

CLMsgLoopManagerForLibevent_test.o : ./CLMsgLoopManagerForLibevent_test.cpp
	g++ -I ../include -o CLMsgLoopManagerForLibevent_test.o -c CLMsgLoopManagerForLibevent_test.cpp -g
