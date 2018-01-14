#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <modbus.h>
#include "test.h"

int main(int argc, char*argv[]){
    modbus_t *ctx;
    uint8_t *query;
    int rc;
    int s = -1;

    ctx = modbus_new_tcp("127.0.0.1", 1502);
    modbus_set_slave(ctx, SERVER_ID);
    s = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &s);
        
    modbus_mapping_t *mb_mapping = NULL;
    mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                    MODBUS_MAX_READ_REGISTERS, 0);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
       
    // server loop
    for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

        rc = modbus_receive(ctx, query);
        if (rc > 0) {
            modbus_reply(ctx, query, rc, mb_mapping);
        } else if (rc  == -1) {
            /* Connection closed by the client or error */
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    modbus_mapping_free(mb_mapping);
    if (s != -1) {
        close(s);
    }
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
