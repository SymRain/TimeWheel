TimeWheelLib.a: TimeWheel.o TimeWheelCppApi.o
	ar -cr TimeWheelLib.a TimeWheel.o TimeWheelCppApi.o
TimeWheel.o: ./include/TimeWheel.h ./lib/TimeWheel.cpp
	g++ -c ./lib/TimeWheel.cpp  -o TimeWheel.o -lpthread
TimeWheelCppApi.o: ./lib/TimeWheelApi.cpp ./include/TimeWheel_Api.h
	g++ -c ./lib/TimeWheelApi.cpp -o TimeWheelCppApi.o -lpthread

clean:
	rm -rf TimeWheelLib.a TimeWheel.o TimeWheelCppApi.o