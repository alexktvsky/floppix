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
    hcnse_socket_t fd;
    socklen_t addrlen;
    bool done;
    hcnse_err_t err;

    fd = HCNSE_INVALID_SOCKET;
    hcnse_memset(listener, 0, sizeof(hcnse_listener_t));
    hcnse_memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;
    done = false;

    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err,
            "getaddrinfo() failed to %s:%s", ip, port);
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        err = hcnse_socket_init(&fd,
            rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (err != HCNSE_OK) {
            continue;
        }
        err = hcnse_socket_nopush(fd);
        if (err != HCNSE_OK) {
            hcnse_socket_close(fd);
            continue;
        }
        err = hcnse_socket_nonblocking(fd);
        if (err != HCNSE_OK) {
            hcnse_socket_close(fd);
            continue;
        }

        for (size_t tries = 0; tries < 5; tries++) {
            err = hcnse_socket_bind(fd, rp->ai_addr, rp->ai_addrlen);
            if (err != HCNSE_OK) {
                hcnse_log_error1(HCNSE_LOG_INFO, HCNSE_OK,
                    "try again to bind after 500ms");
                hcnse_msleep(500);
            }
            else {
                done = true;
                break;
            }
        }
        /* Stop search, we found available address */
        if (done)
            break;
        else {
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "still could not bind to %s:%s", ip, port);
            hcnse_socket_close(fd);
            continue;
        }
    }
    if (!rp) {
        goto failed;
    }

    addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr,
            &addrlen) != 0)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err,
                "getsockname() failed to %s:%s", ip, port);
        
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
        hcnse_socket_close(fd);
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
    hcnse_socket_t fd;
    socklen_t addrlen;
    bool done;
    hcnse_err_t err;

    fd = HCNSE_INVALID_SOCKET;
    hcnse_memset(listener, 0, sizeof(hcnse_listener_t));
    hcnse_memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;
    done = false;

    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_EMERG, err,
            "getaddrinfo() failed to %s:%s", ip, port);
        goto failed;
    }

    for (rp = result; rp; rp = rp->ai_next) {
        err = hcnse_socket_init(&fd,
            rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (err != HCNSE_OK) {
            continue;
        }
        err = hcnse_socket_nopush(fd);
        if (err != HCNSE_OK) {
            hcnse_socket_close(fd);
            continue;
        }
        err = hcnse_socket_nonblocking(fd);
        if (err != HCNSE_OK) {
            hcnse_socket_close(fd);
            continue;
        }
        for (size_t tries = 0; tries < 5; tries++) {
            err = hcnse_socket_bind(fd, rp->ai_addr, rp->ai_addrlen);
            if (err != HCNSE_OK) {
                hcnse_log_error1(HCNSE_LOG_INFO, HCNSE_OK,
                    "try again to bind after 500ms");
                hcnse_msleep(500);
            }
            else {
                done = true;
                break;
            }
        }
        /* Stop search, we found available address */
        if (done)
            break;
        else {
            hcnse_log_error1(HCNSE_LOG_ERROR, err,
                "still could not bind to %s:%s", ip, port);
            hcnse_socket_close(fd);
            continue;
        }
    }
    if (!rp) {
        goto failed;
    }

    addrlen = sizeof(struct sockaddr_storage);
    if (getsockname(fd, (struct sockaddr *) &listener->sockaddr,
            &addrlen) != 0)
    {
        err = hcnse_get_socket_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "getsockname() failed to %s:%s", ip, port);
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
        hcnse_socket_close(fd);
    }
    return err;
}

hcnse_err_t 
hcnse_listener_open(hcnse_listener_t *listener)
{
    hcnse_err_t err;

    err = hcnse_socket_listen(listener->fd, HCNSE_MAX_CONNECT_QUEUELEN);
    if (err != HCNSE_OK) {
        return err;
    }

    return HCNSE_OK;
}

void
hcnse_listener_close(hcnse_listener_t *listener)
{
    if (listener->fd != HCNSE_INVALID_SOCKET) {
        hcnse_socket_close(listener->fd);
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

    fd = HCNSE_INVALID_SOCKET;
    addrlen = sizeof(struct sockaddr_in);

    err = hcnse_socket_accept(&fd, listener->fd,
        (struct sockaddr *) &connect->sockaddr, &addrlen);
    if (err != HCNSE_OK) {
        return err;
    }
    err = hcnse_socket_nonblocking(fd);
    if (err != HCNSE_OK) {
        hcnse_socket_close(fd);
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
        hcnse_socket_shutdown(connect->fd, HCNSE_SHUT_RDWR);
        hcnse_socket_close(connect->fd);
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
hcnse_sockaddr_get_addr_text(const struct sockaddr *sockaddr)
{
    static _Thread_local char buf[100];
    socklen_t addrlen;

    addrlen = sizeof(struct sockaddr_storage);

    if (getnameinfo(sockaddr, addrlen, buf, NI_MAXHOST, 0, 0,
        NI_NUMERICHOST) != 0)
    {
        return NULL;
    }

    return buf;
}

const char *
hcnse_sockaddr_get_port_text(const struct sockaddr *sockaddr)
{
    static _Thread_local char buf[100];
    socklen_t addrlen;

    addrlen = sizeof(struct sockaddr_storage);

    if (getnameinfo(sockaddr, addrlen, NULL, 0, buf, NI_MAXSERV,
        NI_NUMERICHOST|NI_NUMERICSERV) != 0)
    {
        return NULL;
    }

    return buf;
}

const char *
hcnse_listener_get_ip_addr(hcnse_listener_t *listener)
{
    return hcnse_sockaddr_get_addr_text((const struct sockaddr *)
        &(listener->sockaddr));
}

const char *
hcnse_listener_get_port(hcnse_listener_t *listener)
{
    return hcnse_sockaddr_get_port_text((const struct sockaddr *)
        &(listener->sockaddr));
}

const char *
hcnse_connection_get_ip_addr(hcnse_connect_t *connect)
{
    return hcnse_sockaddr_get_addr_text((const struct sockaddr *)
        &(connect->sockaddr));
}

const char *
hcnse_connection_get_port(hcnse_connect_t *connect)
{
    return hcnse_sockaddr_get_port_text((const struct sockaddr *)
        &(connect->sockaddr));
}
