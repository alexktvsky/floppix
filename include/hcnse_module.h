#ifndef INCLUDED_HCNSE_MODULE_H
#define INCLUDED_HCNSE_MODULE_H

#include "hcnse_portable.h"
#include "hcnse_common.h"


struct hcnse_module_s {
    char *name; /* check loading by name and strcmp */
    uint32_t version;
    hcnse_conf_directive_t *directives;

    void *conf;
    void *(*conf_create)(hcnse_cycle_t *cycle);
    hcnse_err_t (*conf_init)(hcnse_cycle_t *cycle, void *conf);
    void (*conf_fini)(hcnse_cycle_t *cycle, void *conf);
};


/* XXX: cycle->modules */


/* void hcnse_module_load(int argc, char **argv); */
/* hcnse_err_t hcnse_module_unload(void); */

#endif /* INCLUDED_HCNSE_MODULE_H */
