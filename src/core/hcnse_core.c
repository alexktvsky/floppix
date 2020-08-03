#include "hcnse_portable.h"
#include "hcnse_core.h"


hcnse_err_t
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

hcnse_err_t
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


hcnse_err_t
hcnse_handler_listen(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    hcnse_listen_t *listen;
    hcnse_pool_t *pool;

    pool = params->server->pool;

    listen = hcnse_pcalloc(pool, sizeof(hcnse_listen_t));
    if (!listen) {
        return hcnse_get_errno();
    }





    // listen->text_addr = "0.0.0.0";

    return HCNSE_OK;
}

void *
hcnse_core_preinit(hcnse_server_t *server)
{
    /*
     * Context of core module has already allocated, so we just
     * return the current pointer
     */

    /*
     * Next fields set by hcnse_pcalloc()
     *
     * server->workdir = NULL;
     * server->priority = 0;
     *
     */

    server->daemon = -1;

    return server;
}


hcnse_err_t
hcnse_core_init(hcnse_server_t *server, void *data)
{
    /* 
     * For core module contex is a server run time context
     */

    if (server != data) {
        return HCNSE_FAILED;
    }


    if (server->daemon == -1) {
        server->daemon = 1;
    }


#if (HCNSE_POSIX)

    if (!server->workdir) {
        server->workdir = "/";
    }

#elif (HCNSE_WIN32)

    if (!server->workdir) {
        server->workdir = "C:\\";
    }

#endif

    return HCNSE_OK;
}


hcnse_command_t hcnse_core_cmd[] = {
    {"import", HCNSE_TAKE1, hcnse_handler_import, 0},
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
    {"listen", HCNSE_TAKE1, hcnse_handler_listen, 0},

    /* {"log", HCNSE_TAKE2|HCNSE_TAKE3, foo, 0}, */
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
