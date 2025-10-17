all: server client

server:EpollServer.cpp
	g++ -o server EpollServer.cpp

client:Client.cpp
	g++ -o client Client.cpp

.PHONY: clean

clean:
	rm -f server client
	