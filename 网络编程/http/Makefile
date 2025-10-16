# 声明伪目标
.PHONY: all clean

# 默认目标
all:  server

# 编译 server
server: Server.cpp
	g++ -o $@ $^ 

# 清理生成文件
clean:
	rm -f server
