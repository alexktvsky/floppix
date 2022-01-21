#ifndef FLOPPIX_CORE_PARSE_H
#define FLOPPIX_CORE_PARSE_H

#include "floppix/system/os/portable.h"
#include "floppix/system/types.h"
#include "floppix/system/pool.h"
#include "floppix/core/forwards.h"

fpx_ssize_t fpx_config_parse_size(const char *str);
fpx_uint_t fpx_config_parse_addr_port(char **addr, char **port, const char *str,
    fpx_pool_t *pool);
fpx_uint_t fpx_config_parse_log_level(const char *str);

#endif /* FLOPPIX_CORE_PARSE_H */
