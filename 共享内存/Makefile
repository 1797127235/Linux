.PHONY: all
all : client server
client: client.cpp
	g++ -o $@ $^ -std=c++20
server: server.cpp
	g++ -o $@ $^ -std=c++20
.PHONY: clean
clean:
	rm -f client server