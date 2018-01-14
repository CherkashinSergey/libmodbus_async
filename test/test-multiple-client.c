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
    
    uint8_t *query;
    int rc;

    struct modbus_async_t *devices[AMOUNT_OF_DEVICES];
    for(int i = 0; i < AMOUNT_OF_DEVICES; i++){
        modbus_t *ctx;
        ctx = modbus_new_tcp("127.0.0.1", PORT_RANGE_BEGIN+i);
        if (ctx == NULL) {
            fprintf(stderr, "Unable to allocate libmodbus context in %d iteration\n", i);
            return -1;
        }
        modbus_set_slave(ctx, SERVER_ID + i);
        rc = modbus_connect(ctx);
        if (rc == -1) {
            fprintf(stderr, "Unable to connect in %d iteration: %s\n", i, modbus_strerror(errno));
            modbus_free(ctx);
            return -1;
        }

        struct modbus_async_t *device = modbus_async_new(ctx);
        devices[i] = device;
    }

    for(int i = 0; i < AMOUNT_OF_DEVICES; i++){
        modbus_async_execute_func_int(devices[i], &async_func, devices[i]);
    }

    for(int i = 0; i < AMOUNT_OF_DEVICES; i++){
        modbus_async_wait_execution(devices[i]);
        if (modbus_async_get_retval(devices[i]) == TEST_RET_VALUE){
            printf("Device %d returned correct value\n", i);
        }else{
            printf("Failed to return correct value from thread.\n");
        }
    }

    for(int i = 0; i < AMOUNT_OF_DEVICES; i++){
        modbus_async_close_connection(devices[i]);
        modbus_async_free(devices[i]);
    }
    
    return 0;
}
