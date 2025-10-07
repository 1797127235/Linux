.PHONY:all
all:server client
server:UdpServerMain.cpp
	g++ -o server UdpServerMain.cpp
client:Client.cpp
	g++ -o client Client.cpp
.PHONY:clean
clean:
	rm -f server client