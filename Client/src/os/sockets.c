#include "platform.h"
#include "sockets.h"


int close_socket(socket_t socket) {
#if (SYSTEM_LINUX)
    return close(socket);
#elif (SYSTEM_FREEBSD)
    return close(socket);
#elif (SYSTEM_WIN32) || (SYSTEM_WIN64)
    return closesocket(socket);
#endif
}


int init_winsock(void) {
#if (SYSTEM_WIN32) || (SYSTEM_WIN64)
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
    return 0;
#endif
}
