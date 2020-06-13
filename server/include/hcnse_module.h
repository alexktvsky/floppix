#ifndef INCLUDED_HCNSE_MODULE_H
#define INCLUDED_HCNSE_MODULE_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


struct hcnse_module_s {
    void *init;
    void *fini;
    hcnse_conf_directive_t *directives;
};


hcnse_err_t hcnse_module_load(void);
hcnse_err_t hcnse_module_unload(void);

#endif /* INCLUDED_HCNSE_MODULE_H */
