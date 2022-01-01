#ifndef FPX_CORE_MODULE_H
#define FPX_CORE_MODULE_H

#include "fpx.system.errno.h"
#include "fpx.core.server.h"
#include "fpx.core.command.h"

#define FPX_MODULE_RUN_TIME_PART \
    NULL, \
    NULL, \
    0, \
    0

/*
#define fpx_use_module(name) \
    extern fpx_module_t fpx_##name##_module;

#define fpx_declare_module(name, version, cmd, create_cntx, init_cntx, fini_cntx) \
    fpx_use_module(name); \
    fpx_module_t fpx_##name##_module
*/




typedef struct fpx_module_s fpx_module_t;

struct fpx_module_s {
    char *name; /* check loading by name and strcmp */
    uint32_t version;
    fpx_command_t *cmd;
    void *(*preinit)(fpx_server_t *server);
    fpx_err_t (*init)(fpx_server_t *server, void *cntx);
    void (*fini)(fpx_server_t *server, void *cntx);
    void *cntx;
    void *handle;
    bool preinited;
    bool inited;
};


fpx_err_t fpx_setup_prelinked_modules(fpx_server_t *server);
fpx_err_t fpx_preinit_modules(fpx_server_t *server);
fpx_err_t fpx_init_modules(fpx_server_t *server);
void fpx_fini_modules(fpx_server_t *server);

fpx_module_t *fpx_load_module(fpx_server_t *server, const char *fname);
void fpx_unload_module(fpx_server_t *server, fpx_module_t *module);

#endif /* FPX_CORE_MODULE_H */
