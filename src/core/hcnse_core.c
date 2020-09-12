#include "hcnse_portable.h"
#include "hcnse_core.h"


static hcnse_err_t
foo(hcnse_cmd_params_t *params, void *data, int argc, char **argv)
{
    int i;
    (void) data;
    printf("%s ", params->directive->name);
    for (i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return HCNSE_OK;
}

static hcnse_err_t
hcnse_handler_include(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    hcnse_pool_t *pool;
    hcnse_config_t *config;
    hcnse_err_t err;

    (void) data;
    (void) argc;

    config = params->config;
    pool = params->server->pool;

    err = hcnse_config_read_included(config, pool, argv[0]);
    if (err != HCNSE_OK) {
        return err;
    }

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
        return hcnse_get_errno();
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
    hcnse_uint_t level;
    ssize_t size;

    (void) data;

    level = hcnse_config_parse_log_level(argv[1]);
    if (level == HCNSE_LOG_INVALID_LEVEL) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
            "%s:%zu: \"%s\" is unknown log level",
            params->directive->filename, params->directive->line, argv[1]);
        return HCNSE_ERR_CONFIG_SYNTAX;        
    }

    if (argc == 2 && hcnse_strcmp(argv[0], "stdout") == 0) {
        hcnse_logger_add_log_fd(params->server->logger, level, HCNSE_STDOUT);
        return HCNSE_OK;
    }
    else if (argc == 2 && hcnse_strcmp(argv[0], "stderr") == 0) {
        hcnse_logger_add_log_fd(params->server->logger, level, HCNSE_STDERR);
    }
    else if (argc == 2 && hcnse_strcmp(argv[0], "syslog") == 0) {
        hcnse_log_error1(HCNSE_LOG_WARN, HCNSE_OK,
            "Syslog is not available now");
    }
    else if (argc == 2) {
        hcnse_logger_add_log_file(params->server->logger, level, argv[0], 0);
    }

    if (argc == 3) {
        size = hcnse_config_parse_size(argv[2]);
        if (size == -1) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "%s:%zu: Invalid argument \"%s\" in log directive",
                params->directive->filename, params->directive->line, argv[2]);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }

        hcnse_logger_add_log_file(params->server->logger, level, argv[0], size);
    }

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
    hcnse_logger_t *logger;

    hcnse_assert(listeners = hcnse_list_create(server->pool));
    hcnse_assert(connections = hcnse_list_create(server->pool));
    hcnse_assert(free_connections = hcnse_list_create(server->pool));
    hcnse_assert(logger = hcnse_logger_create());

    server->daemon = 1;
    server->workdir = HCNSE_DEFAULT_WORKDIR;
    server->priority = HCNSE_DEFAULT_PRIORITY;
    server->user = HCNSE_DEFAULT_USER;
    server->group = HCNSE_DEFAULT_GROUP;
    
    server->logger = logger;

    server->listeners = listeners;
    server->connections = connections;
    server->free_connections = free_connections;

    return server;
}


static hcnse_err_t
hcnse_core_init(hcnse_server_t *server, void *data)
{
    hcnse_listener_t *listener;
    hcnse_list_node_t *iter;
    hcnse_err_t err;

    /* For core module contex is a server run time context */
    hcnse_assert(server == data);

    if (hcnse_list_size(server->listeners) == 0) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
            "Listen address and port not specified");
        return HCNSE_FAILED;
    }

    iter = hcnse_list_first(server->listeners);
    for ( ; iter; iter = iter->next) {
        listener = (hcnse_listener_t *) (iter->data);

        if ((err = hcnse_listener_bind(listener)) != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, err, "Listener (%s:%s) failed",
                listener->text_addr, listener->text_port);
            return HCNSE_FAILED;
        }

        if ((err = hcnse_listener_open(listener)) != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, err, "Listener (%s:%s) failed",
                listener->text_addr, listener->text_port);
            return HCNSE_FAILED;
        }
        hcnse_pool_cleanup_add(server->pool, listener, hcnse_listener_close);
    }

    if ((err = hcnse_process_set_workdir(server->workdir)) != HCNSE_OK) {
        goto failed;
    }

    if (server->user != HCNSE_DEFAULT_USER) {
        if ((err = hcnse_process_set_user(server->user)) != HCNSE_OK) {
            goto failed;
        }
    }

    if (server->group != HCNSE_DEFAULT_GROUP) {
        if ((err = hcnse_process_set_group(server->group)) != HCNSE_OK) {
            goto failed;
        }
    }




    if ((err = hcnse_logger_start(server->logger)) != HCNSE_OK) {
        goto failed;
    }


    /*
     * Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon
     */

    if (server->daemon) {
        if ((err = hcnse_process_become_daemon()) != HCNSE_OK) {
            goto failed;
        }
    }



    return HCNSE_OK;

failed:
    return err;
}


hcnse_command_t hcnse_core_cmd[] = {
    {"include", HCNSE_TAKE1, hcnse_handler_include, 0},
    {"import", HCNSE_TAKE1, hcnse_handler_import, 0},
    {"listen", HCNSE_TAKE1, hcnse_handler_listen, 0},
    {"log", HCNSE_TAKE2|HCNSE_TAKE3, hcnse_handler_log, 0},
    {"daemon", HCNSE_TAKE1, hcnse_handler_flag,
        offsetof(hcnse_server_t, daemon)},
    {"workdir", HCNSE_TAKE1, hcnse_handler_str,
        offsetof(hcnse_server_t, workdir)},
    {"priority", HCNSE_TAKE1, hcnse_handler_uint,
        offsetof(hcnse_server_t, priority)},
    {"user", HCNSE_TAKE1, hcnse_handler_str,
        offsetof(hcnse_server_t, user)},
    {"group", HCNSE_TAKE1, hcnse_handler_str,
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
