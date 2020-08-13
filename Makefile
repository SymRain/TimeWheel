libsrtimewheel.a.0.1: TimeWheel.o TimeWheelCppApi.o
	ar -cr libsrtimewheel.a.0.1 TimeWheel.o TimeWheelCppApi.o 
TimeWheel.o: ./include/TimeWheel.h ./lib/TimeWheel.cpp ./include/TimeWheelPubDef.h
	g++ -c ./lib/TimeWheel.cpp  -o TimeWheel.o -lpthread -I ./include -std=c++11
TimeWheelCppApi.o: ./lib/TimeWheelApi.cpp ./include/TimeWheel_Api.h ./include/TimeWheelPubDef.h
	g++ -c ./lib/TimeWheelApi.cpp -o TimeWheelCppApi.o -I ./include -lpthread -std=c++11

install:
	cp ./include/TimeWheel_Api.h /usr/local/include
	cp ./include/TimeWheelPubDef.h /usr/local/include
	cp ./libsrtimewheel.a.0.1 /usr/local/lib/libsrtimewheel.a.0.1
	ln -s /usr/local/lib/libsrtimewheel.a.0.1 /usr/local/lib/libsrtimewheel.a
uninstall:
	rm -rf /usr/local/include/TimeWheel_Api.h /usr/local/lib/libsrtimewheel.a.0.1
	rm -rf /usr/local/lib/libsrtimewheel.a
clean:
	rm -rf libsrtimewheel.a.0.1 TimeWheel.o TimeWheelCppApi.o 