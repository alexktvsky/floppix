#ifndef HCNSE_CORE_PARSE_H
#define HCNSE_CORE_PARSE_H

#include "hcnse.system.os.portable.h"
#include "hcnse.system.pool.h"
#include "hcnse.type.forwards.h"

ssize_t hcnse_config_parse_size(const char *str);
hcnse_uint_t hcnse_config_parse_addr_port(char **addr, char **port,
    const char *str, hcnse_pool_t *pool);
hcnse_uint_t hcnse_config_parse_log_level(const char *str);

#endif /* HCNSE_CORE_PARSE_H */
