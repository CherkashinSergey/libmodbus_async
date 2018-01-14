LIBMODBUS_INCLUDE_PATH=/home/zloj/monocub/bus/libmodbus_x86/libmodbus/src
LIBMODBUS_LIB_PATH=/home/zloj/monocub/bus/libmodbus_x86/libmodbus/src/.libs

TEST_SRC = 	test-client.c \
			test-server.c \
			test-multiple-client.c \
			test-multiple-server.c 

TEST_EXEC = $(TEST_SRC:.c= )

all: static shared tests

static:
	mkdir -p lib
	gcc -I $(LIBMODBUS_INCLUDE_PATH) -L $(LIBMODBUS_LIB_PATH) -c -Wall -Werror -fpic src/modbus_async.c -o lib/modbus_async.o -lpthread -lmodbus
	ar  rcs lib/libmodbus_async.a  lib/modbus_async.o
	rm lib/modbus_async.o

shared:
	mkdir -p lib
	gcc -I $(LIBMODBUS_INCLUDE_PATH) -c -fPIC src/modbus_async.c -o lib/modbus_async.o
	gcc -shared -Wl,-soname,lib/libmodbus_async.so.1 -o lib/libmodbus_async.so  lib/modbus_async.o
	rm lib/modbus_async.o

tests: 
	$(MAKE) -C test

.PHONY : clean
clean:
	rm -rf lib/*
	$(MAKE) -C test clean
