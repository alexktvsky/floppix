#ifndef INCLUDED_HCNSE_MODULE_H
#define INCLUDED_HCNSE_MODULE_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


#define HCNSE_MODULE_RUN_TIME_PART \
    NULL, \
    NULL, \
    0, \
    0

/*
#define hcnse_use_module(name) \
    extern hcnse_module_t hcnse_##name##_module;

#define hcnse_declare_module(name, version, cmd, create_cntx, init_cntx, fini_cntx) \
    hcnse_use_module(name); \
    hcnse_module_t hcnse_##name##_module
*/






struct hcnse_module_s {
    char *name; /* check loading by name and strcmp */
    uint32_t version;
    hcnse_command_t *cmd;
    void *(*preinit)(hcnse_server_t *server);
    hcnse_err_t (*init)(hcnse_server_t *server, void *cntx);
    void (*fini)(hcnse_server_t *server, void *cntx);
    void *cntx;
    void *handle;
    hcnse_flag_t preinited;
    hcnse_flag_t inited;
};


hcnse_err_t hcnse_setup_prelinked_modules(hcnse_server_t *server);
hcnse_err_t hcnse_preinit_modules(hcnse_server_t *server);
hcnse_err_t hcnse_init_modules(hcnse_server_t *server);
void hcnse_fini_modules(hcnse_server_t *server);

hcnse_module_t *hcnse_load_module(hcnse_server_t *server, const char *fname);
void hcnse_unload_module(hcnse_server_t *server, hcnse_module_t *module);

#endif /* INCLUDED_HCNSE_MODULE_H */
