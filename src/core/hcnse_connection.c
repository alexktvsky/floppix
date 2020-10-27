#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_err_t
hcnse_connection_init(hcnse_connect_t *connect)
{
    hcnse_pool_t *pool;

    hcnse_memzero(connect, sizeof(hcnse_connect_t));

    pool = hcnse_pool_create(0, NULL);
    if (!pool) {
        return hcnse_get_errno();
    }

    connect->type_id = HCNSE_CONNECTION_ID;
    connect->fd = HCNSE_INVALID_SOCKET;
    connect->pool = pool;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_connection_accept(hcnse_connect_t *connect, hcnse_listener_t *listener)
{
    hcnse_socket_t fd;
    hcnse_socklen_t addrlen;
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
    /* connect->ready_to_write = 0; */
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
hcnse_connection_clear(hcnse_connect_t *connect)
{
    hcnse_connection_close(connect);
    hcnse_pool_clear(connect->pool);
}

void
hcnse_connection_destroy(hcnse_connect_t *connect)
{
    hcnse_connection_close(connect);
    hcnse_pool_destroy(connect->pool);
}

const char *
hcnse_connection_get_addr_text(hcnse_connect_t *connect, char *buf,
    size_t bufsize)
{
    return hcnse_sockaddr_get_addr_text(
        (const struct sockaddr *) &(connect->sockaddr), buf, bufsize);
}

const char *
hcnse_connection_get_port_text(hcnse_connect_t *connect, char *buf,
    size_t bufsize)
{
    return hcnse_sockaddr_get_port_text(
        (const struct sockaddr *) &(connect->sockaddr), buf, bufsize);
}
