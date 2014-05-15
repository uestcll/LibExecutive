test : test.o CLMsgLoopManagerForLibevent.o
	g++ -o test test.o CLMsgLoopManagerForLibevent.o ../libexecutive.a -g -I../include -levent -lpthread -luuid

test.o : test.cpp
	g++ -o test.o -c test.cpp -I../include -g -levent

CLMsgLoopManagerForLibevent.o : CLMsgLoopManagerForLibevent.cpp
	g++ -o CLMsgLoopManagerForLibevent.o -c CLMsgLoopManagerForLibevent.cpp -I../include -g -levent
