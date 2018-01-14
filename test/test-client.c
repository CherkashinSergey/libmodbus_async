#include <stdio.h>
#include <errno.h>
#include <modbus.h>
#include <modbus_async.h>
#include "test.h"

int async_func (void* args){
    printf("Hello from async_func!\n");
    struct modbus_async_t * device = (struct modbus_async_t * )args;
    modbus_t *ctx = device->ctx;
    uint16_t reg[5];// will store read registers values

    //Read 5 holding registers starting from address 10
    int num = modbus_read_registers(ctx, 10, 5, reg);
    if (num != 5) {// number of read registers is not the one expected
        fprintf(stderr, "Failed to read: %s\n", modbus_strerror(errno));
        return -1;
    }else{
        printf("modbus_read_registers executed successfully\n");
    }
    int retval = TEST_RET_VALUE;
    return retval;
}

int main(int argc, char*argv[]){
    modbus_t *ctx;
    uint8_t *query;
    int rc;

    ctx = modbus_new_tcp("127.0.0.1", 1502);
    if (ctx == NULL) {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }
    modbus_set_slave(ctx, SERVER_ID);
    rc = modbus_connect(ctx);
    if (rc == -1) {
        fprintf(stderr, "Unable to connect %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    struct modbus_async_t *device = modbus_async_new(ctx);
    modbus_async_execute_func_int(device, &async_func, device);
    modbus_async_wait_execution(device);
    if (modbus_async_get_retval(device) == TEST_RET_VALUE){
        printf("Returned value is correct\n");
    }else{
        printf("Failed to return correct value from thread.\n");
    }

    modbus_async_close_connection(device);
    modbus_async_free(device);
    
    return 0;
}
