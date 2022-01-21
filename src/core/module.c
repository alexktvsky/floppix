#include "floppix/core/module.h"
#include "floppix/util/string.h"
#include "floppix/system/dso.h"

extern fpx_module_t fpx_core_module;

fpx_module_t *fpx_preloaded_modules[] = {
    &fpx_core_module,
};

static fpx_err_t
fpx_add_module(fpx_server_t *server, fpx_module_t *module)
{
    fpx_module_t *module_iter;

    fpx_list_foreach (node, &server->modules) {
        module_iter = fpx_list_data(node, fpx_module_t, list_node);
        if (!fpx_strcmp(module_iter->name, module->name)) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_FAILED,
                "Module \"%s\" is already plugged", module->name);
            return FPX_FAILED;
        }
    }

    fpx_list_push_back(&server->modules, &module->list_node);

    return FPX_OK;
}

fpx_err_t
fpx_setup_prelinked_modules(fpx_server_t *server)
{
    fpx_size_t n, i;
    fpx_err_t err;

    n = sizeof(fpx_preloaded_modules) / sizeof(fpx_module_t *);

    for (i = 0; i < n; ++i) {
        err = fpx_add_module(server, fpx_preloaded_modules[i]);
        if (err != FPX_OK) {
            return err;
        }
    }

    return FPX_OK;
}

fpx_err_t
fpx_preinit_modules(fpx_server_t *server)
{
    fpx_module_t *module;
    void *cntx;

    cntx = NULL;

    fpx_list_foreach (node, &server->modules) {
        module = fpx_list_data(node, fpx_module_t, list_node);
        if (module->preinited) {
            break;
        }
        if (module->preinit) {
            cntx = module->preinit(server);
            if (!cntx) {
                return FPX_FAILED;
            }
        }

        module->cntx = cntx;
        module->preinited = 1;
    }

    return FPX_OK;
}

fpx_err_t
fpx_init_modules(fpx_server_t *server)
{
    fpx_module_t *module;
    fpx_err_t err;

    fpx_list_foreach (node, &server->modules) {
        module = fpx_list_data(node, fpx_module_t, list_node);
        if (module->inited) {
            break;
        }
        if (module->init) {
            err = module->init(server, module->cntx);
        }
        if (err != FPX_OK) {
            return FPX_FAILED;
        }
        module->inited = 1;
    }

    return FPX_OK;
}

fpx_err_t
fpx_modules_fini(fpx_server_t *server)
{
    fpx_module_t *module;

    fpx_list_foreach (node, &server->modules) {
        module = fpx_list_data(node, fpx_module_t, list_node);
        if (module->fini) {
            module->fini(server, module->cntx);
        }
    }

    return FPX_OK;
}

fpx_module_t *
fpx_load_module(fpx_server_t *server, const char *fname)
{
    char errsetr[FPX_ERRNO_STR_SIZE];
    void *handle;
    fpx_module_t *module;

    fpx_err_t err;

    handle = NULL;

    err = fpx_check_absolute_path(fname);
    if (err != FPX_OK) {
        fpx_log_error1(FPX_LOG_ERROR, err, "Failed to load module \"%s\"",
            fname);
        goto failed;
    }

    handle = fpx_dlopen(fname);
    if (!handle) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "fpx_dlopen() failed (%d: \"%s\")",
            err, fpx_dlerror(errsetr, FPX_ERRNO_STR_SIZE));
        err = FPX_FAILED;
        goto failed;
    }

    module = fpx_dlsym(handle, "fpx_module");
    if (!module) {
        err = fpx_get_errno();
        fpx_log_error1(FPX_LOG_ERROR, err, "fpx_dlsym() failed (%d: \"%s\")",
            err, fpx_dlerror(errsetr, FPX_ERRNO_STR_SIZE));
        err = FPX_FAILED;
        goto failed;
    }

    module->handle = handle;

    /*
        fpx_pool_cleanup_add(server->pool, handle, fpx_dlclose);
    */
    err = fpx_add_module(server, module);
    if (err != FPX_OK) {
        goto failed;
    }

    return module;

failed:
    fpx_set_errno(err);
    if (handle) {
        fpx_dlclose(handle);
    }
    return NULL;
}

void
fpx_unload_module(fpx_server_t *server, fpx_module_t *module)
{
    module->fini(server, module->cntx);
    if (module->handle) {
        fpx_dlclose(module->handle);
    }
}
