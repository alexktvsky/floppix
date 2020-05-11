#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_LISTENER_FLAG    1
#define HCNSE_CONNECTION_FLAG  2


hcnse_err_t
hcnse_listener_init_ipv4(hcnse_listener_t *listener, const char *ip,
    const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    hcnse_socket_t fd = HCNSE_INVALID_SOCKET;
    hcnse_err_t err;


    hcnse_memset(listener, 0, sizeof(hcnse_listener_t));
    hcnse_memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        hcnse_log_error1(HCNSE_LOG_EMERG, hcnse_get_errno(),
            "getaddrinfo() failed to %s:%s", ip, port);
        err = hcnse_get_socket_errno();
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == HCNSE_INVALID_SOCKET) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "getaddrinfo() failed to %s:%s", ip, port);
            err = hcnse_get_socket_errno();
            continue;
        }
        err = hcnse_tcp_nopush(fd);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "hcnse_tcp_nopush() failed to %s:%s", ip, port);
            hcnse_close_socket(fd);
            continue;
        }
        err = hcnse_socket_nonblocking(fd);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "hcnse_socket_nonblocking() failed to %s:%s", ip, port);
            hcnse_close_socket(fd);
            continue;
        }
        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == -1) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "bind() failed to %s:%s", ip, port);
            hcnse_close_socket(fd);
            err = hcnse_get_socket_errno();
            continue;
        }
        /* Stop search, we found available address */
        else {
            break;
        }
    }
    if (!rp) {
        goto failed;
    }

    socklen_t addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr,
            &addrlen) != 0)
    {
        hcnse_log_error1(HCNSE_LOG_EMERG, hcnse_get_errno(),
                "getsockname() failed to %s:%s", ip, port);
        err = HCNSE_ERR_NET_GSN;
        goto failed;
    }
    listener->identifier = HCNSE_LISTENER_FLAG;
    listener->fd = fd;
    freeaddrinfo(result);

    return HCNSE_OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != HCNSE_INVALID_SOCKET) {
        hcnse_close_socket(fd);
    }
    return err;
}

hcnse_err_t
hcnse_listener_init_ipv6(hcnse_listener_t *listener, const char *ip,
    const char *port)
{
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct addrinfo *rp;
    hcnse_socket_t fd = HCNSE_INVALID_SOCKET;
    hcnse_err_t err;


    hcnse_memset(listener, 0, sizeof(hcnse_listener_t));
    hcnse_memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        hcnse_log_error1(HCNSE_LOG_EMERG, hcnse_get_errno(),
            "getaddrinfo() failed to %s:%s", ip, port);
        err = HCNSE_ERR_NET_GAI;
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == HCNSE_INVALID_SOCKET) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "socket() failed to %s:%s", ip, port);
            err = hcnse_get_socket_errno();
            continue;
        }
        err = hcnse_tcp_nopush(fd);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "hcnse_tcp_nopush() failed to %s:%s", ip, port);
            hcnse_close_socket(fd);
            continue;
        }
        err = hcnse_socket_nonblocking(fd);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "hcnse_socket_nonblocking() failed to %s:%s", ip, port);
            hcnse_close_socket(fd);
            continue;
        }
        if (bind(fd, rp->ai_addr, rp->ai_addrlen) == -1) {
            hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
                "bind() failed to %s:%s", ip, port);
            hcnse_close_socket(fd);
            err = hcnse_get_socket_errno();
            continue;
        }
        /* Stop search, we found available address */
        else {
            break;
        }
    }
    if (!rp) {
        goto failed;
    }

    socklen_t addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr,
            &addrlen) != 0)
    {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "getsockname() failed to %s:%s", ip, port);
        err = HCNSE_ERR_NET_GSN;
        goto failed;
    }
    listener->identifier = HCNSE_LISTENER_FLAG;
    listener->fd = fd;
    freeaddrinfo(result);

    return HCNSE_OK;

failed:
    if (result) {
        freeaddrinfo(result);
    }
    if (fd != HCNSE_INVALID_SOCKET) {
        hcnse_close_socket(fd);
    }
    return err;
}

hcnse_err_t 
hcnse_listener_open(hcnse_listener_t *listener)
{
    if (listen(listener->fd, HCNSE_MAX_CONNECT_QUEUELEN) == -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "listen() failed to %s:%s", hcnse_listener_get_addr(listener),
            hcnse_listener_get_port(listener));
        return hcnse_get_socket_errno();
    }

    return HCNSE_OK;
}

void
hcnse_listener_close(hcnse_listener_t *listener)
{
    if (listener->fd != HCNSE_INVALID_SOCKET) {
        hcnse_close_socket(listener->fd);
        listener->fd = HCNSE_INVALID_SOCKET;
    }
}

void
hcnse_listener_clean(hcnse_listener_t *listener)
{
    hcnse_listener_close(listener);
    hcnse_memset(listener, 0, sizeof(hcnse_listener_t));
}

hcnse_err_t
hcnse_connection_init(hcnse_connect_t *connect)
{
    hcnse_pool_t *pool;

    hcnse_memset(connect, 0, sizeof(hcnse_connect_t));

    pool = hcnse_pool_create(NULL);
    if (!pool) {
        return hcnse_get_errno();
    }

    connect->identifier = HCNSE_CONNECTION_FLAG;
    connect->fd = HCNSE_INVALID_SOCKET;
    connect->pool = pool;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_connection_accept(hcnse_connect_t *connect, hcnse_listener_t *listener)
{
    hcnse_socket_t fd;
    socklen_t addrlen;
    hcnse_err_t err;

    addrlen = sizeof(struct sockaddr_in);

    fd = accept(listener->fd, (struct sockaddr *)&connect->sockaddr, &addrlen);
    if (fd == HCNSE_INVALID_SOCKET) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "accept() failed to %s:%s", hcnse_listener_get_addr(listener),
            hcnse_listener_get_port(listener));
        return hcnse_get_socket_errno();
    }
    err = hcnse_socket_nonblocking(fd);
    if (err != HCNSE_OK) {
        hcnse_close_socket(fd);
        return err;
    }
    connect->fd = fd;
    connect->owner = listener;
    // connect->ready_to_write = 0;
    return HCNSE_OK;
}

void
hcnse_connection_close(hcnse_connect_t *connect)
{
    if (connect->fd != HCNSE_INVALID_SOCKET) {
        hcnse_close_socket(connect->fd);
        connect->fd = HCNSE_INVALID_SOCKET;
    }
}

void
hcnse_connection_clean(hcnse_connect_t *connect)
{
    hcnse_connection_close(connect);
    hcnse_pool_clean(connect->pool);
}

void
hcnse_connection_destroy(hcnse_connect_t *connect)
{
    hcnse_connection_close(connect);
    hcnse_pool_destroy(connect->pool);
}

bool
hcnse_is_listener(void *instance)
{
    struct temp {
        int8_t identifier;
    };
    struct temp *p = (struct temp *) instance;
    return ((p->identifier) == HCNSE_LISTENER_FLAG);
}

bool
hcnse_is_connection(void *instance)
{
    struct temp {
        int8_t identifier;
    };
    struct temp *p = (struct temp *) instance;
    return ((p->identifier) == HCNSE_CONNECTION_FLAG);
}


const char *
hcnse_connection_get_addr(hcnse_connect_t *connect)
{
    static _Thread_local char buf[100];
    struct sockaddr_storage *sockaddr;

    sockaddr = &(connect->sockaddr);
    socklen_t addrlen = sizeof(struct sockaddr_storage);

    if (getnameinfo((const struct sockaddr *)sockaddr, addrlen,
                            buf, NI_MAXHOST, 0, 0, NI_NUMERICHOST) != 0) {
        return NULL;
    }
    return buf;
}

const char *
hcnse_connection_get_port(hcnse_connect_t *connect)
{
    static _Thread_local char buf[100];
    struct sockaddr_storage *sockaddr;
    socklen_t addrlen;

    sockaddr = &(connect->sockaddr);
    addrlen = sizeof(struct sockaddr_storage);

    if (getnameinfo((const struct sockaddr *)sockaddr, addrlen, NULL, 0,
                    buf, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV) != 0) {
        return NULL;
    }
    return buf;
}

