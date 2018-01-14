LIBMODBUS_INCLUDE_PATH=/home/zloj/monocub/bus/libmodbus_x86/libmodbus/src
LIBMODBUS_LIB_PATH=/home/zloj/monocub/bus/libmodbus_x86/libmodbus/src/.libs

TEST_SRC = 	test-client.c \
			test-server.c \
			test-multiple-client.c \
			test-multiple-server.c 

TEST_EXEC = $(TEST_SRC:.c= )

tests: $(TEST_EXEC)

$(TEST_EXEC):
	gcc -static -I $(LIBMODBUS_INCLUDE_PATH) -I ../src -L $(LIBMODBUS_LIB_PATH) -L ../lib $@.c -o $@ -lmodbus -lmodbus_async -lpthread

.PHONY : clean
clean:
	rm -rf $(TEST_EXEC)