libtimewheel.a: TimeWheel.o TimeWheelCppApi.o
	ar -cr libtimewheel.a TimeWheel.o TimeWheelCppApi.o 
Test.o: test.cpp
	g++ -c test.cpp  -o Test.o -lpthread -std=c++11 -g
TimeWheel.o: ./include/TimeWheel.h ./lib/TimeWheel.cpp
	g++ -c ./lib/TimeWheel.cpp  -o TimeWheel.o -lpthread -std=c++11 -g
TimeWheelCppApi.o: ./lib/TimeWheelApi.cpp ./include/TimeWheel_Api.h
	g++ -c ./lib/TimeWheelApi.cpp -o TimeWheelCppApi.o -lpthread -std=c++11 -g

install:
cp ./include/TimeWheel_Api.h /usr/local/include
cp ./libtimewheel.a /usr/local/lib/libtimewheel.a

uninstall:
rm -rf /usr/local/include/TimeWheel_Api.h /usr/local/lib/libtimewheel.a

clean:
	rm -rf libtimewheel.a TimeWheel.o TimeWheelCppApi.o 