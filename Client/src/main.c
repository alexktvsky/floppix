#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "error_proc.h"
#include "sockets.h"
#include "connect.h"


int main(void) {
    //InitWinSocketsLib();

    client_connect_unit_s ccunit1 = {"0.0.0.0", 2100};
    if (init_connect_tcp(&ccunit1) != SUCCESS) {
        abort();
    }

    fprintf(stdout, "Connected to server\n");
    
    char message[] = "Hello, world!";
    send(ccunit1.socket, message, sizeof(message), 0);

    /* Soft close connect */
    shutdown(ccunit1.socket, SHUT_BOTH);
    CloseSocket(ccunit1.socket);
    return 0;
}
