#ifndef INCLUDED_HCNSE_PARSE_H
#define INCLUDED_HCNSE_PARSE_H


ssize_t hcnse_config_parse_size(const char *str);
hcnse_uint_t hcnse_config_parse_addr_port(char **addr, char **port,
    const char *str, hcnse_pool_t *pool);
hcnse_uint_t hcnse_config_parse_log_level(const char *str);

#endif /* INCLUDED_HCNSE_PARSE_H */
