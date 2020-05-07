#ifndef INCLUDED_HCNSE_MODULE_H
#define INCLUDED_HCNSE_MODULE_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


struct hcnse_module_s {


};


hcnse_err_t hcnse_module_load(void);
hcnse_err_t hcnse_module_init(void);
hcnse_err_t hcnse_module_unload(void);

#endif /* INCLUDED_HCNSE_MODULE_H */
