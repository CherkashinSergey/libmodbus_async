#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <modbus.h>
#include <modbus_async.h>
#include "test.h"

modbus_mapping_t *mb_mapping = NULL; //temporary global

int async_server_loop(void * args){
    struct modbus_async_t * device = (struct modbus_async_t * )args;
    int i = device->ret_buff;
    int s = -1;
    device->ctx = modbus_new_tcp("127.0.0.1", PORT_RANGE_BEGIN+i);
        if (device->ctx == NULL) {
            fprintf(stderr, "Unable to allocate libmodbus context in %d iteration\n", i);
            return -1;
        }
        modbus_set_slave(device->ctx, SERVER_ID + i);
        s = modbus_tcp_listen(device->ctx, 1);
        modbus_tcp_accept(device->ctx, &s);
    int rc;
    // server loop
    for (;;) {
        
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

        rc = modbus_receive(device->ctx, query);
        if (rc > 0) {
            modbus_reply(device->ctx, query, rc, mb_mapping);
        } else if (rc  == -1) {
            /* Connection closed by the client or error */
            break;
        }
    }
    modbus_close(device->ctx);
    if (s == -1){
        close(s);
    }
    device->state=CLOSING;
    return 0;
}

int main(int argc, char*argv[]){
    int rc;
    int s_dev[AMOUNT_OF_DEVICES];
    struct modbus_async_t *devices[AMOUNT_OF_DEVICES];
    int number_of_running_servers = 0;

    for(int i = 0; i < AMOUNT_OF_DEVICES; i++){
        s_dev[i]=-1;
        modbus_t *ctx;
        
        devices[i] = modbus_async_new(ctx);
        if(devices[i]){
            devices[i]->ret_buff = i;
            modbus_async_execute_func_int(devices[i], &async_server_loop, devices[i]);
            number_of_running_servers++;
        }else{
            fprintf(stderr, "Unable to allocate modbus_async in %d iteration\n", i);
            return -1;
        }
    }
        
    //modbus_mapping_t *mb_mapping = NULL; //TODO: make it without using global variable
    mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                    MODBUS_MAX_READ_REGISTERS, 0);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        for(int i = 0; i < AMOUNT_OF_DEVICES; i++){
            modbus_async_free(devices[i]);
        }
        return -1;
    }

    printf("Started main server loop\n");
       
    // server loop
    do {
        for(int i = 0; i < AMOUNT_OF_DEVICES; i++){
            if(devices[i]){
                if (devices[i]->state == CLOSING){
                    printf("Device %d: thread is stopped\n", i);
                    modbus_async_free(devices[i]);
                    devices[i] = NULL;
                    number_of_running_servers--;
                    if (s_dev[i] != -1) {
                        close(s_dev[i]);
                    }
                    printf("Number of running servers: %d\n", number_of_running_servers);
                }
            }
        }
    }while(number_of_running_servers > 0);

    printf("Quit the main loop.\n");

    modbus_mapping_free(mb_mapping);

    return 0;
}
