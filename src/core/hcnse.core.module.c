#include "hcnse.core.module.h"
#include "hcnse.util.string.h"
#include "hcnse.system.dso.h"

extern hcnse_module_t hcnse_core_module;


hcnse_module_t *hcnse_preloaded_modules[] = {
    &hcnse_core_module
};


static hcnse_err_t
hcnse_add_module(hcnse_server_t *server, hcnse_module_t *module)
{
    hcnse_module_t *current;
    hcnse_list_node_t *node;

    node = server->modules->head;

    for ( ; node; node = node->next) {
        current = (hcnse_module_t *) node->data;
        if (!hcnse_strcmp(current->name, module->name)) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
                "Module \"%s\" is already plugged", module->name);
            return HCNSE_FAILED;
        }
    }

    hcnse_list_push_back(server->modules, module);

    return HCNSE_OK;
}

hcnse_err_t
hcnse_setup_prelinked_modules(hcnse_server_t *server)
{
    hcnse_uint_t n, i;
    hcnse_err_t err;

    n = sizeof(hcnse_preloaded_modules) / sizeof(hcnse_module_t *);

    for (i = 0; i < n; ++i) {
        err = hcnse_add_module(server, hcnse_preloaded_modules[i]);
        if (err != HCNSE_OK) {
            return err;
        }
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_preinit_modules(hcnse_server_t *server)
{
    hcnse_module_t *module;
    hcnse_list_node_t *node;
    void *cntx;

    node = server->modules->head;

    for ( ; node; node = node->next) {

        module = (hcnse_module_t *) node->data;

        if (module->preinited) {
            break;
        }

        if (module->preinit) {
            cntx = module->preinit(server);
        }

        if (!cntx) {
            return HCNSE_FAILED;
        }

        module->cntx = cntx;
        module->preinited = 1;
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_init_modules(hcnse_server_t *server)
{
    hcnse_module_t *module;
    hcnse_list_node_t *node;
    hcnse_err_t err;

    node = server->modules->head;

    for ( ; node; node = node->next) {

        module = (hcnse_module_t *) node->data;

        if (module->inited) {
            break;
        }

        if (module->init) {
            err = module->init(server, module->cntx);
        }

        if (err != HCNSE_OK) {
            return HCNSE_FAILED;
        }

        module->inited = 1;
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_modules_fini(hcnse_server_t *server)
{
    hcnse_module_t *module;
    hcnse_list_node_t *node;

    node = server->modules->head;
    for ( ; node; node = node->next) {
        module = (hcnse_module_t *) node->data;
        if (module->fini) {
            module->fini(server, module->cntx);
        }
    }

    return HCNSE_OK;
}


hcnse_module_t *
hcnse_load_module(hcnse_server_t *server, const char *fname)
{
    char errsetr[HCNSE_ERRNO_STR_SIZE];
    void *handle;
    hcnse_module_t *module;

    hcnse_err_t err;


    handle = NULL;

    err = hcnse_check_absolute_path(fname);
    if (err != HCNSE_OK) {
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "Failed to load module \"%s\"", fname);
        goto failed;
    }

    handle = hcnse_dlopen(fname);
    if (!handle) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "hcnse_dlopen() failed (%d: \"%s\")",
            err, hcnse_dlerror(errsetr, HCNSE_ERRNO_STR_SIZE));
        err = HCNSE_FAILED;
        goto failed;
    }

    module = hcnse_dlsym(handle, "hcnse_module");
    if (!module) {
        err = hcnse_get_errno();
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "hcnse_dlsym() failed (%d: \"%s\")",
            err, hcnse_dlerror(errsetr, HCNSE_ERRNO_STR_SIZE));
        err = HCNSE_FAILED;
        goto failed;
    }

    module->handle = handle;

/*
    hcnse_pool_cleanup_add(server->pool, handle, hcnse_dlclose);
*/
    err = hcnse_add_module(server, module);
    if (err != HCNSE_OK) {
        goto failed;
    }

    return module;

failed:
    hcnse_set_errno(err);
    if (handle) {
        hcnse_dlclose(handle);
    }
    return NULL;
}

void
hcnse_unload_module(hcnse_server_t *server, hcnse_module_t *module)
{
    module->fini(server, module->cntx);
    if (module->handle) {
        hcnse_dlclose(module->handle);
    }
}
