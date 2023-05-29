# Compiler to use
CC = clang++-14

# Compiler flags
CFLAGS = -pthread -std=c++14

# Shared Library flags
LDFLAGS = -shared

# List of source files
SOURCES = server.cpp 

# Output executables
SERVER_OUT = react_server
LIB_OUT = libreactor.so

all: $(SERVER_OUT) $(CLIENT_OUT) $(LIB_OUT)

$(SERVER_OUT): server.cpp
	$(CC) $(CFLAGS) -o $(SERVER_OUT) server.cpp -ldl

$(LIB_OUT): reactor.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(LIB_OUT) reactor.cpp

clean:
	rm -f $(SERVER_OUT) $(LIB_OUT)