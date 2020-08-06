#include "hcnse_portable.h"
#include "hcnse_core.h"


static hcnse_err_t
foo(hcnse_cmd_params_t *params, void *data, int argc, char **argv)
{
    int i;
    (void) data;
    printf("%s ", params->directive->name);
    for (i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return HCNSE_OK;
}

static hcnse_err_t
hcnse_handler_import(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    hcnse_module_t *module;
    (void) data;
    (void) argc;

    module = hcnse_load_module(params->server, argv[0]);
    if (!module) {
        return HCNSE_FAILED;
    }

    return hcnse_preinit_modules(params->server);
}

static hcnse_err_t
hcnse_handler_listen(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    hcnse_listener_t *listener;
    hcnse_pool_t *pool;
    char *addr, *port;
    hcnse_uint_t rv;
    hcnse_err_t err;

    (void) data;
    (void) argc;

    pool = params->server->pool;

    listener = hcnse_palloc(pool, sizeof(hcnse_listener_t));
    if (!listener) {
        return hcnse_get_errno();
    }

    rv = hcnse_config_parse_addr_port(&addr, &port, argv[0], pool);
    if (rv == HCNSE_ADDR_IPV4) {
        err = hcnse_listener_init_ipv4(listener, addr, port);
        if (err != HCNSE_OK) {
            return err;
        }
    }
    else if (rv == HCNSE_ADDR_IPV6) {
        err = hcnse_listener_init_ipv6(listener, addr, port);
        if (err != HCNSE_OK) {
            return err;
        }
    }
    else {
        return HCNSE_ERR_CONFIG_SYNTAX;
    }

    hcnse_list_push_back(params->server->listeners, listener);

    return HCNSE_OK;
}

static hcnse_err_t
hcnse_handler_log(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{

    return HCNSE_OK;
}

static void *
hcnse_core_preinit(hcnse_server_t *server)
{
    /*
     * Context of core module has already allocated, so we just
     * return the current pointer
     */

    hcnse_list_t *listeners;
    hcnse_list_t *connections;
    hcnse_list_t *free_connections;

    // hcnse_logger_t *logger;

    listeners = hcnse_list_create(server->pool);
    if (!listeners) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "Failed to allocate list of listeners");
        return NULL;
    }

    connections = hcnse_list_create(server->pool);
    if (!connections) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "Failed to allocate list of connections");
        return NULL;
    }

    free_connections = hcnse_list_create(server->pool);
    if (!free_connections) {
        hcnse_log_error1(HCNSE_LOG_ERROR, hcnse_get_errno(),
            "Failed to allocate list of free connections");
        return NULL;
    }

    // hcnse_logger_create();


    /*
     * Next fields set by hcnse_pcalloc()
     *
     * server->workdir = NULL;
     * server->user = NULL;
     * server->group = NULL;
     * server->priority = 0;
     *
     */

    server->daemon = -1;

    server->listeners = listeners;
    server->connections = connections;
    server->free_connections = free_connections;
    // server->logger = logger;

    return server;
}


static hcnse_err_t
hcnse_core_init(hcnse_server_t *server, void *data)
{
    hcnse_listener_t *listener;
    hcnse_list_node_t *iter;
    hcnse_err_t err;

    /* For core module contex is a server run time context */
    if (server != data) { // TODO: change to hcnse_assert()
        return HCNSE_FAILED;
    }

    if (hcnse_list_size(server->listeners) == 0) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
            "Listen address and port not specified");
        return HCNSE_FAILED;
    }

    iter = hcnse_list_first(server->listeners);
    for ( ; iter; iter = iter->next) {
        listener = (hcnse_listener_t *) (iter->data);
        err = hcnse_listener_open(listener);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, err, "Failed to listen %s:%s",
                listener->text_addr, listener->text_port);
            return HCNSE_FAILED;
        }
        hcnse_pool_cleanup_add(server->pool, listener, hcnse_listener_close);
    }

    if (!server->workdir) {
#if (HCNSE_POSIX)
        server->workdir = "/";
#elif (HCNSE_WIN32)
        server->workdir = "C:\\";
#endif
    }
    err = hcnse_process_set_workdir(server->workdir);
    if (err != HCNSE_OK) {
        goto failed;
    }

    if (!server->user) {
        // server->user = 
    }
    // err = hcnse_process_set_user(server->user);
    // if (err != HCNSE_OK) {
    //     goto failed;
    // }











    /*
     * Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon
     */
    if (server->daemon == -1) {
        server->daemon = 1;
    }
    if (server->daemon) {
        hcnse_process_become_daemon();
    }




    return HCNSE_OK;

failed:
    return err;
}


hcnse_command_t hcnse_core_cmd[] = {
    {"import", HCNSE_TAKE1, hcnse_handler_import, 0},
    {"listen", HCNSE_TAKE1, hcnse_handler_listen, 0},
    {"log", HCNSE_TAKE2|HCNSE_TAKE3, hcnse_handler_log, 0},
    {"daemon", HCNSE_TAKE1, hcnse_handler_set_flag,
        offsetof(hcnse_server_t, daemon)},
    {"workdir", HCNSE_TAKE1, hcnse_handler_set_str,
        offsetof(hcnse_server_t, workdir)},
    {"priority", HCNSE_TAKE1, hcnse_handler_set_uint,
        offsetof(hcnse_server_t, priority)},
    {"user", HCNSE_TAKE1, hcnse_handler_set_str,
        offsetof(hcnse_server_t, user)},
    {"group", HCNSE_TAKE1, hcnse_handler_set_str,
        offsetof(hcnse_server_t, group)},
    HCNSE_NULL_COMMAND
};

hcnse_module_t hcnse_core_module = {
    "core",
    0x00000003,
    hcnse_core_cmd,
    hcnse_core_preinit,
    hcnse_core_init,
    NULL,
    HCNSE_MODULE_RUN_TIME_PART
};
