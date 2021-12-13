#CC=g++ -g -O3 -DNDEBUG
CC=g++ -g
PROTOC=$(PROTOBUF)/protoc

PROTOBUF=./protobuf-3.18.1/src
LIB=$(PROTOBUF)/.libs/libprotobuf.a -ldl -pthread
INC=-I $(PROTOBUF) -I $(HEADERS)
HEADERS=inc
BIN=bin
SRC=src

COMMON_O=$(BIN)/kv.pb.o $(BIN)/log.o $(BIN)/protocol.o $(BIN)/rpc.o

all: bin client simple_client server

bin:
	mkdir $(BIN)

# binaries and main object files

simple_client: simple_client.o common
	$(CC) -o simple_client $(BIN)/simple_client.o $(COMMON_O) $(LIB)

simple_client.o: src/simple_client.cpp common
	$(CC) -c $(SRC)/simple_client.cpp -o $(BIN)/simple_client.o $(INC)

client: client.o common
	$(CC) -o client $(BIN)/client.o $(COMMON_O) $(LIB)

client_simple_runner: client.o
	$(CC) -o client $(BIN)/client.o $(COMMON_O) $(LIB)

client.o: src/client.cpp common
	$(CC) -c $(SRC)/client.cpp -o $(BIN)/client.o $(INC)

server: PersistentHashTable.o server.o common
	$(CC) -o server $(BIN)/server.o $(BIN)/PersistentHashTable.o $(COMMON_O) $(LIB)

server_simple_runner: PersistentHashTable.o server.o
	$(CC) -o server $(BIN)/server.o $(BIN)/PersistentHashTable.o $(COMMON_O) $(LIB)

server.o: PersistentHashTable.o src/server.cpp common
	$(CC) -c $(SRC)/server.cpp -o $(BIN)/server.o $(INC)

PersistentHashTable.o:
	$(CC) -c $(SRC)/PersistentHashTable.cpp -o $(BIN)/PersistentHashTable.o $(INC)

# libs

common: kv log protocol rpc

log: inc/log.h src/log.cpp
	$(CC) -c $(SRC)/log.cpp -o $(BIN)/log.o $(INC)

kv: kv.proto
	$(PROTOC) --cpp_out=. kv.proto
	$(CC) -c kv.pb.cc -o $(BIN)/kv.pb.o $(INC)

protocol: inc/protocol.h src/protocol.cpp
	$(CC) -c $(SRC)/protocol.cpp -o $(BIN)/protocol.o $(INC)

rpc: inc/rpc.h src/rpc.cpp
	$(CC) -c $(SRC)/rpc.cpp -o $(BIN)/rpc.o $(INC)

#clean
clean:
	rm -rf $(BIN) client server

