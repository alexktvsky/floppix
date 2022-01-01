#include "fpx.core.server.h"
#include "fpx.system.os.portable.h"
#include "fpx.system.errno.h"
#include "fpx.system.process.h"
#include "fpx.util.string.h"


#if 0
static fpx_err_t
foo(fpx_cmd_params_t *params, void *data, fpx_int_t argc, char **argv)
{
    fpx_int_t i;
    (void) data;
    printf("%s ", params->directive->name);
    for (i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    return FPX_OK;
}
#endif

static fpx_err_t
fpx_handler_include(fpx_cmd_params_t *params, void *data, fpx_int_t argc,
    char **argv)
{
    fpx_pool_t *pool;
    fpx_config_t *config;
    fpx_err_t err;

    (void) data;
    (void) argc;

    config = params->config;
    pool = params->server->pool;

    err = fpx_config_read_included(config, pool, argv[0]);
    if (err != FPX_OK) {
        return err;
    }

    return FPX_OK;
}

static fpx_err_t
fpx_handler_import(fpx_cmd_params_t *params, void *data, fpx_int_t argc,
    char **argv)
{
    fpx_module_t *module;

    (void) data;
    (void) argc;

    module = fpx_load_module(params->server, argv[0]);
    if (!module) {
        return fpx_get_errno();
    }

    return fpx_preinit_modules(params->server);
}

static fpx_err_t
fpx_handler_listen(fpx_cmd_params_t *params, void *data, fpx_int_t argc,
    char **argv)
{
    fpx_listener_t *listener;
    fpx_pool_t *pool;
    char *addr, *port;
    fpx_uint_t rv;
    fpx_err_t err;

    (void) data;
    (void) argc;

    pool = params->server->pool;

    listener = fpx_palloc(pool, sizeof(fpx_listener_t));
    if (!listener) {
        return fpx_get_errno();
    }

    rv = fpx_config_parse_addr_port(&addr, &port, argv[0], pool);
    if (rv == FPX_ADDR_IPV4) {
        err = fpx_listener_init_ipv4(listener, addr, port);
        if (err != FPX_OK) {
            return err;
        }
    }
    else if (rv == FPX_ADDR_IPV6) {
        err = fpx_listener_init_ipv6(listener, addr, port);
        if (err != FPX_OK) {
            return err;
        }
    }
    else {
        return FPX_ERR_CONFIG_SYNTAX;
    }

    fpx_list_push_back(params->server->listeners, listener);

    return FPX_OK;
}

static fpx_err_t
fpx_handler_log(fpx_cmd_params_t *params, void *data, fpx_int_t argc,
    char **argv)
{
    fpx_uint_t level;
    fpx_ssize_t size;

    (void) data;

    level = fpx_config_parse_log_level(argv[1]);
    if (level == FPX_LOG_INVALID_LEVEL) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
            "%s:%zu: \"%s\" is unknown log level",
            params->directive->filename, params->directive->line, argv[1]);
        return FPX_ERR_CONFIG_SYNTAX;
    }

    if (argc == 2 && fpx_strcmp(argv[0], "stdout") == 0) {
        if (params->server->daemon) {
            fpx_log_error1(FPX_LOG_WARN, FPX_OK,
                "stdout is not available in daemon mode");
        }
        fpx_logger_add_log_fd(params->server->logger, level, FPX_STDOUT);
        return FPX_OK;
    }
    else if (argc == 2 && fpx_strcmp(argv[0], "stderr") == 0) {
        if (params->server->daemon) {
            fpx_log_error1(FPX_LOG_WARN, FPX_OK,
                "stderr is not available in daemon mode");
        }
        fpx_logger_add_log_fd(params->server->logger, level, FPX_STDERR);
    }
    else if (argc == 2 && fpx_strcmp(argv[0], "syslog") == 0) {
        fpx_log_error1(FPX_LOG_WARN, FPX_OK,
            "Syslog is not available now");
    }
    else if (argc == 2) {
        fpx_logger_add_log_file(params->server->logger, level, argv[0], 0);
    }

    if (argc == 3) {
        size = fpx_config_parse_size(argv[2]);
        if (size == -1) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "%s:%zu: Invalid argument \"%s\" in log directive",
                params->directive->filename, params->directive->line, argv[2]);
            return FPX_ERR_CONFIG_SYNTAX;
        }

        fpx_logger_add_log_file(params->server->logger, level, argv[0], size);
    }

    return FPX_OK;
}

static void *
fpx_core_preinit(fpx_server_t *server)
{
    /*
     * Context of core module has already allocated, so we just
     * return the current pointer
     */

    fpx_list_t *listeners;
    fpx_list_t *connections;
    fpx_list_t *free_connections;
    fpx_logger_t *logger;

    fpx_assert(fpx_list_init(&listeners, server->pool) == FPX_OK);
    fpx_assert(fpx_list_init(&connections, server->pool) == FPX_OK);
    fpx_assert(fpx_list_init(&free_connections, server->pool) == FPX_OK);
    fpx_assert(fpx_logger_init(&logger) == FPX_OK);

    server->daemon = true;
    server->workdir = FPX_DEFAULT_WORKDIR;
    server->priority = FPX_DEFAULT_PRIORITY;
    server->user = FPX_DEFAULT_USER;
    server->group = FPX_DEFAULT_GROUP;
    
    server->logger = logger;

    server->listeners = listeners;
    server->connections = connections;
    server->free_connections = free_connections;

    return server;
}


static fpx_err_t
fpx_core_init(fpx_server_t *server, void *data)
{
    fpx_listener_t *listener;
    fpx_list_node_t *node;
    fpx_err_t err;

    /* For core module contex is a server run time context */
    fpx_assert(server == data);

    if (server->listeners->size == 0) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_FAILED,
            "Listen address and port not specified");
        return FPX_FAILED;
    }

    node = server->listeners->head;
    for ( ; node; node = node->next) {
        listener = (fpx_listener_t *) node->data;

        if ((err = fpx_listener_bind(listener)) != FPX_OK) {
            fpx_log_error1(FPX_LOG_ERROR, err, "Listener (%s:%s) failed",
                listener->text_addr, listener->text_port);
            return FPX_FAILED;
        }

        if ((err = fpx_listener_open(listener)) != FPX_OK) {
            fpx_log_error1(FPX_LOG_ERROR, err, "Listener (%s:%s) failed",
                listener->text_addr, listener->text_port);
            return FPX_FAILED;
        }
        fpx_pool_cleanup_add(server->pool, listener, fpx_listener_close);
    }

    if ((err = fpx_process_set_workdir(server->workdir)) != FPX_OK) {
        goto failed;
    }

    if (server->user != FPX_DEFAULT_USER) {
        if ((err = fpx_process_set_user(server->user)) != FPX_OK) {
            goto failed;
        }
    }

    if (server->group != FPX_DEFAULT_GROUP) {
        if ((err = fpx_process_set_group(server->group)) != FPX_OK) {
            goto failed;
        }
    }




    if ((err = fpx_logger_start(server->logger)) != FPX_OK) {
        goto failed;
    }


    /*
     * Now log file is available and server can write error mesages in it, so
     * here we close TTY, fork off the parent process and run daemon
     */

    if (server->daemon) {
        if ((err = fpx_process_become_daemon()) != FPX_OK) {
            goto failed;
        }
    }



    return FPX_OK;

failed:
    return err;
}


fpx_command_t fpx_core_cmd[] = {
    {"include", FPX_TAKE1, fpx_handler_include, 0},
    {"import", FPX_TAKE1, fpx_handler_import, 0},
    {"listen", FPX_TAKE1, fpx_handler_listen, 0},
    {"log", FPX_TAKE2|FPX_TAKE3, fpx_handler_log, 0},
    {"daemon", FPX_TAKE1, fpx_handler_flag,
        offsetof(fpx_server_t, daemon)},
    {"workdir", FPX_TAKE1, fpx_handler_str,
        offsetof(fpx_server_t, workdir)},
    {"priority", FPX_TAKE1, fpx_handler_uint,
        offsetof(fpx_server_t, priority)},
    {"user", FPX_TAKE1, fpx_handler_str,
        offsetof(fpx_server_t, user)},
    {"group", FPX_TAKE1, fpx_handler_str,
        offsetof(fpx_server_t, group)},
    FPX_NULL_COMMAND
};

fpx_module_t fpx_core_module = {
    "core",
    0x00000004,
    fpx_core_cmd,
    fpx_core_preinit,
    fpx_core_init,
    NULL,
    FPX_MODULE_RUN_TIME_PART
};
