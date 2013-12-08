main : CLTimerMsgObserver.o TimerServer.o 
	g++ -o main CLTimerMsgObserver.o TimerServer.o  -L../ -lexecutive -g -lpthread
	rm *.o

TimerServer.o : TimerServer.cpp
	g++ -o TimerServer.o -c TimerServer.cpp -L../ -lexecutive -I../include -g

CLTimerMsgObserver.o : CLTimerMsgObserver.cpp
	g++ -o CLTimerMsgObserver.o -c CLTimerMsgObserver.cpp -L../ -lexecutive -I../include -g




