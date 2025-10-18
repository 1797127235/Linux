all:server client

server:main.cpp
	g++ -o $@ $^ -std=c++17 -I/usr/include/jsoncpp -ljsoncpp

client:Client.cpp
	g++ -o $@ $^ -std=c++17 -I/usr/include/jsoncpp -ljsoncpp

.PHONY:clean

clean:
	rm -f server client


