# main : ZTester.o CLSocketServerTester.o  test.o  ATester.o 
# 	g++ -I../../../../gtest-1.6.0/include -o main ZTester.o CLSocketServerTester.o test.o ATester.o -L../../../../gtest-1.6.0/lib/ -lgtest  -L../  -lexecutive  -g -lpthread
# 	rm *.o
main : CLSocketServerTester.o  test.o 
	g++ -I../../../../../gtest-1.6.0/include -o main  CLSocketServerTester.o test.o -L../../../../../gtest-1.6.0/lib/ -lgtest  -L../../../ -lexecutive  -g -lpthread
	rm *.o


# ATester.o : ATester.cpp
# 	g++ -I../../../../gtest-1.6.0/include -o ATester.o -c ATester.cpp -L../ -lexecutive -I../include -g

CLSocketServerTester.o : CLSocketServerTester.cpp
	g++ -I../../../../../gtest-1.6.0/include -o CLSocketServerTester.o -c CLSocketServerTester.cpp -L../../../ -lexecutive -I../../../include -g

test.o : test.cpp
	g++ -I../../../../../gtest-1.6.0/include -o test.o -c test.cpp -L../../../ -lexecutive -I../../../include -g

# ZTester.o : ZTester.cpp
# 	g++ -I../../../../gtest-1.6.0/include -o ZTester.o -c ZTester.cpp -L../ -lexecutive -I../include -g
