#ifndef FPX_CORE_PARSE_H
#define FPX_CORE_PARSE_H

#include "fpx.system.os.portable.h"
#include "fpx.system.type.h"
#include "fpx.system.pool.h"
#include "fpx.core.forwards.h"

fpx_ssize_t fpx_config_parse_size(const char *str);
fpx_uint_t fpx_config_parse_addr_port(char **addr, char **port, const char *str,
    fpx_pool_t *pool);
fpx_uint_t fpx_config_parse_log_level(const char *str);

#endif /* FPX_CORE_PARSE_H */
