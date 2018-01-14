#ifndef LIBMODBUS_ASYNC_H
#define LIBMODBUS_ASYNC_H
#include <modbus.h>

#define DEBUG_MODE 0
#if DEBUG_MODE
# define DEBUG_MSG(fmt,args...)	printf(fmt, ##args)
#else
# define DEBUG_MSG(fmt,args...)
#endif

enum device_state{
    WAITING,
    EXEC_INT,
    EXEC_VOID,
    CLOSING
};

struct modbus_async_t {
    pthread_t tid;
    modbus_t *ctx;
    enum device_state state;
    int (*func_to_execute)(void*);
    void* func_args;
    int ret_buff;
};

/* Allocates memory and initializes */
struct modbus_async_t* modbus_async_new(modbus_t * ctx);

/* Creates a task for child thread */
void modbus_async_execute_func_int(struct modbus_async_t * as, void * func, void * args);

/* Makes main thread wait for child thread to execute function as->func_to_execute.
 * Resets stored result of previously executed function.
 */
void modbus_async_wait_execution(struct modbus_async_t * as);

/* Returns a pointer to value returned by as->func_to_execute.
 * Should be casted to corresponding data type manually!!!
*/
int modbus_async_get_retval(struct modbus_async_t * as);

/* Returns a state of as */
enum device_state modbus_async_get_state(struct modbus_async_t * as);

/* Sends command to child thread to finish and closes as->ctx connection */
int modbus_async_close_connection(struct modbus_async_t * as);

/* Free allocated memory */
void modbus_async_free(struct modbus_async_t * as);

#endif /*LIBMODBUS_ASYNC_H*/