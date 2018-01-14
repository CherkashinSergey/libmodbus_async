#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "modbus_async.h"

static void * master_thread (void* args){
    DEBUG_MSG("Hello from the thread\n");
    struct modbus_async_t * device = (struct modbus_async_t * )args;
    DEBUG_MSG("Current operation code: %d\n", device->state);
    while(1){
        switch(device->state){
        case EXEC_INT:
            if (device->func_to_execute != NULL){
                DEBUG_MSG("Func_to_execute performing in separate thread\n");
                device->ret_buff = device->func_to_execute(device->func_args);
                device->state = WAITING;
            }else{
                printf("Empty pointer func_to_execute\n");
            }
            break;
        case EXEC_VOID:
            if (device->func_to_execute != NULL){
                DEBUG_MSG("Func_to_execute performing in separate thread\n");
                device->func_to_execute(device->func_args);
                device->state = WAITING;
            }else{
                printf("Empty pointer func_to_execute\n");
            }
            break;
        case CLOSING:
            DEBUG_MSG("Device thread is stopped\n");
            int retval = 0;
            pthread_exit(&retval);
        default:
            break;
        }
    }

}

struct modbus_async_t* modbus_async_new(modbus_t * ctx){
    struct modbus_async_t* as = (struct modbus_async_t*)malloc(sizeof(struct modbus_async_t));
    as->ctx = ctx;
    as->state = WAITING;
    as->func_to_execute = NULL;
    as->func_args = NULL;
    as->ret_buff = 0;
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&tid,&attr,master_thread, as)){
        fprintf(stderr, "Unable to create thread!\n");
        return NULL;
    }
    as->tid = tid;
    return as;
}

void modbus_async_execute_func_int(struct modbus_async_t * as, void * func, void * args){
    as->func_to_execute = func;
    as->func_args = args;
    as->state=EXEC_INT;
}

void modbus_async_execute_func_void(struct modbus_async_t * as, void * func, void * args){
    as->func_to_execute = func;
    as->func_args = args;
    as->state=EXEC_VOID;
}

void modbus_async_wait_execution(struct modbus_async_t * as){
    if(as){
        do{}
        while(as->state != WAITING);
    }
}

int modbus_async_get_retval(struct modbus_async_t * as){
    return as->ret_buff;
}

enum device_state modbus_async_get_state(struct modbus_async_t * as){
    return as->state;
}

int modbus_async_close_connection(struct modbus_async_t * as){
    if(as){
        as->state = CLOSING;
        int retval = pthread_join(as->tid,NULL);
        DEBUG_MSG("Thread close code %d\n",retval);
        modbus_close(as->ctx);
        return retval;
    }
    return -1;
}

void modbus_async_free(struct modbus_async_t * as){
    if (as){
        modbus_free(as->ctx);
        free(as);
    }
}