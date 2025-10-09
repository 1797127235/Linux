# 声明伪目标
.PHONY: all clean

# 默认目标
all: client server

# 编译 client
client: Client.cpp
	g++ -o $@ $^ -I/usr/include/jsoncpp -ljsoncpp

# 编译 server
server: Server.cpp
	g++ -o $@ $^ -I/usr/include/jsoncpp -ljsoncpp

# 清理生成文件
clean:
	rm -f client server
