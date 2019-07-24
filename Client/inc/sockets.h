#ifndef SOCKETS_H
#define SOCKETS_H

#if (__gnu_linux__)
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <net/if.h>
    #include <netdb.h>
    #define SHUT_RECEIVE SHUT_RD
    #define SHUT_SEND    SHUT_WR
    #define SHUT_BOTH    SHUT_RDWR
    #define CloseSocket(_closeme_) close(_closeme_);
    typedef int socket_t;

#elif (__FreeBSD__)
    /* Details for FreeBSD */

    typedef int socket_t;

#elif (__WIN32__) || (__WIN64__)
    #include <winsock2.h>
    #include <ws2ipdef.h>
    #include <windows.h>
    #define SHUT_RECEIVE SD_RECEIVE
    #define SHUT_SEND    SD_SEND
    #define SHUT_BOTH    SD_BOTH
    #define CloseSocket(_closeme_) closesocket(_closeme_);
    typedef SOCKET socket_t;

#else
    #error "Unsupported operating system!"
#endif

/*
#if defined (__WIN32__) || defined (__WIN64__)
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData)) {
        return error;
    }
#endif
*/

int tcp_nopush(socket_t s);
int tcp_push(socket_t s);

#endif /* SOCKETS_H */
