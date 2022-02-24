#include "floppix/core/connection.h"
#include "floppix/system/memory.h"
#include "floppix/system/net/socket.h"
#include "floppix/system/net/sockopt.h"

fpx_err_t
fpx_connection_init(fpx_connect_t *connect)
{
    fpx_pool_t *pool;
    fpx_err_t err;

    fpx_memzero(connect, sizeof(fpx_connect_t));

    if ((err = fpx_pool_create(&pool, 0, NULL)) != FPX_OK) {
        return err;
    }

    connect->type_id = FPX_CONNECTION_ID;
    connect->fd = FPX_INVALID_SOCKET;
    connect->pool = pool;

    return FPX_OK;
}

fpx_err_t
fpx_connection_accept(fpx_connect_t *connect, fpx_listener_t *listener)
{
    fpx_socket_t fd;
    fpx_socklen_t addrlen;
    fpx_err_t err;

    fd = FPX_INVALID_SOCKET;
    addrlen = sizeof(struct sockaddr_in);

    err = fpx_socket_accept(&fd, listener->fd,
        (struct sockaddr *) &connect->sockaddr, &addrlen);
    if (err != FPX_OK) {
        return err;
    }
    err = fpx_socket_nonblocking(fd);
    if (err != FPX_OK) {
        fpx_socket_close(fd);
        return err;
    }
    connect->fd = fd;
    connect->owner = listener;
    /* connect->ready_to_write = 0; */
    return FPX_OK;
}

void
fpx_connection_close(fpx_connect_t *connect)
{
    if (connect->fd != FPX_INVALID_SOCKET) {
        fpx_socket_shutdown(connect->fd, FPX_SHUT_RDWR);
        fpx_socket_close(connect->fd);
        connect->fd = FPX_INVALID_SOCKET;
    }
}

void
fpx_connection_clear(fpx_connect_t *connect)
{
    fpx_connection_close(connect);
    fpx_pool_clear(connect->pool);
}

void
fpx_connection_destroy(fpx_connect_t *connect)
{
    fpx_connection_close(connect);
    fpx_pool_destroy(connect->pool);
}

const char *
fpx_connection_get_addr_text(fpx_connect_t *connect, char *buf, size_t bufsize)
{
    return fpx_sockaddr_get_addr_text(
        (const struct sockaddr *) &(connect->sockaddr), buf, bufsize);
}

const char *
fpx_connection_get_port_text(fpx_connect_t *connect, char *buf, size_t bufsize)
{
    return fpx_sockaddr_get_port_text(
        (const struct sockaddr *) &(connect->sockaddr), buf, bufsize);
}
