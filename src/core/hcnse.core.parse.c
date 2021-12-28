#include "hcnse.core.parse.h"
#include "hcnse.core.log.h"
#include "hcnse.util.string.h"
#include "hcnse.system.limits.h"
#include "hcnse.system.errno.h"
#include "hcnse.system.net.sockaddr.h"
#include "hcnse.system.memory.h"


ssize_t
hcnse_config_parse_size(const char *str)
{
    char prefix;
    size_t len;
    ssize_t size, scale, max;

    len = hcnse_strlen(str);

    if (len == 0) {
        return -1;
    }

    prefix = str[len - 1];

    switch (prefix) {
    case 'K':
    case 'k':
        len -= 1;
        max = HCNSE_SSIZE_T_MAX / 1024;
        scale = 1024;
        break;

    case 'M':
    case 'm':
        len -= 1;
        max = HCNSE_SSIZE_T_MAX / (1024 * 1024);
        scale = 1024 * 1024;
        break;

    case 'G':
    case 'g':
        len -= 1;
        max = HCNSE_SSIZE_T_MAX / (1024 * 1024 * 1024);
        scale = 1024 * 1024 * 1024;
        break;

    default:
        max = HCNSE_SSIZE_T_MAX;
        scale = 1;
    }

    size = hcnse_atosz(str, len);
    if (size == -1 || size > max) {
        return -1;
    }

    size *= scale;

    return size;
}

hcnse_uint_t
hcnse_config_parse_addr_port(char **addr, char **port, const char *str,
    hcnse_pool_t *pool)
{
    const char *pos, *end, *close_bracket;
    hcnse_int_t int_port;
    hcnse_uint_t rv;
    size_t len;

    len = hcnse_strlen(str);
    end = str + len - 1;
    pos = end;

    while ((pos >= str) && hcnse_isdigit(*pos)) {
        pos -= 1;
    }

    if (pos < str) {
        int_port = hcnse_atoi(str, end - pos);
        if (int_port < 1 || int_port > 65535) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Port value \"%s\" out of range (1-65535)", pos + 1);
            return HCNSE_ADDR_INVALID;
        }
        *port = hcnse_palloc(pool, len + 1);
        hcnse_memmove(*port, str, len + 1); /* Copy with '\0' character */
        *addr = "0.0.0.0"; /* Addr don't specified */
        return HCNSE_ADDR_IPV4;
    }

    if (*pos == ':' && pos < end) {
        if (pos == str) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Unexpected \":\" in the string \"%s\"", str);
            return HCNSE_ADDR_INVALID;
        }
        int_port = hcnse_atoi(pos + 1, end - pos);
        if (int_port < 1 || int_port > 65535) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Port value \"%s\" out of range (1-65535)", pos + 1);
            return HCNSE_ADDR_INVALID;
        }
        len = hcnse_strlen(pos + 1);
        *port = hcnse_palloc(pool, len + 1);
        hcnse_memmove(*port, pos + 1, len + 1); /* Copy with '\0' character */
        end = pos - 1;
    }

    close_bracket = hcnse_strchr(str, ']');

    if (*str == '[') {
        if (!close_bracket || *end != ']') {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Missing \"]\" in the string \"%s\"", str);
            return HCNSE_ADDR_INVALID;
        }
        pos = str + 1;
        len = close_bracket - pos;
        rv = HCNSE_ADDR_IPV6;
    }
    else {
        if (close_bracket) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Unexpected \"]\" in the string \"%s\"", str);
            return HCNSE_ADDR_INVALID;
        }
        pos = str;
        len = end - pos + 1;
        rv = HCNSE_ADDR_IPV4;
    }

    *addr = hcnse_palloc(pool, len + 1);
    hcnse_memmove(*addr, pos, len);
    (*addr)[len] = '\0';

    return rv;
}

hcnse_uint_t
hcnse_config_parse_log_level(const char *str)
{
    hcnse_uint_t i;

    for (i = 0; hcnse_log_prio[i] != NULL; ++i) {
        if (hcnse_strcmp(str, hcnse_log_prio[i]) == 0) {
            return i;
        }
    }

    return HCNSE_LOG_INVALID_LEVEL;
}
