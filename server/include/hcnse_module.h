#ifndef INCLUDED_HCNSE_MODULE_H
#define INCLUDED_HCNSE_MODULE_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


struct hcnse_module_s {


};


hcnse_err_t hcnse_load_module(void);
hcnse_err_t hcnse_init_module(void);
hcnse_err_t hcnse_unload_module(void);

#endif /* INCLUDED_HCNSE_MODULE_H */
