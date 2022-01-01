#include "fpx.core.parse.h"
#include "fpx.core.log.h"
#include "fpx.util.string.h"
#include "fpx.system.type.h"
#include "fpx.system.errno.h"
#include "fpx.system.net.sockaddr.h"
#include "fpx.system.memory.h"


ssize_t
fpx_config_parse_size(const char *str)
{
    char prefix;
    size_t len;
    ssize_t size, scale, max;

    len = fpx_strlen(str);

    if (len == 0) {
        return -1;
    }

    prefix = str[len - 1];

    switch (prefix) {
    case 'K':
    case 'k':
        len -= 1;
        max = FPX_SSIZE_T_MAX / 1024;
        scale = 1024;
        break;

    case 'M':
    case 'm':
        len -= 1;
        max = FPX_SSIZE_T_MAX / (1024 * 1024);
        scale = 1024 * 1024;
        break;

    case 'G':
    case 'g':
        len -= 1;
        max = FPX_SSIZE_T_MAX / (1024 * 1024 * 1024);
        scale = 1024 * 1024 * 1024;
        break;

    default:
        max = FPX_SSIZE_T_MAX;
        scale = 1;
    }

    size = fpx_atosz(str, len);
    if (size == -1 || size > max) {
        return -1;
    }

    size *= scale;

    return size;
}

fpx_uint_t
fpx_config_parse_addr_port(char **addr, char **port, const char *str,
    fpx_pool_t *pool)
{
    const char *pos, *end, *close_bracket;
    fpx_int_t int_port;
    fpx_uint_t rv;
    size_t len;

    len = fpx_strlen(str);
    end = str + len - 1;
    pos = end;

    while ((pos >= str) && fpx_isdigit(*pos)) {
        pos -= 1;
    }

    if (pos < str) {
        int_port = fpx_atoi(str, end - pos);
        if (int_port < 1 || int_port > 65535) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "Port value \"%s\" out of range (1-65535)", pos + 1);
            return FPX_ADDR_INVALID;
        }
        *port = fpx_palloc(pool, len + 1);
        fpx_memmove(*port, str, len + 1); /* Copy with '\0' character */
        *addr = "0.0.0.0"; /* Addr don't specified */
        return FPX_ADDR_IPV4;
    }

    if (*pos == ':' && pos < end) {
        if (pos == str) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "Unexpected \":\" in the string \"%s\"", str);
            return FPX_ADDR_INVALID;
        }
        int_port = fpx_atoi(pos + 1, end - pos);
        if (int_port < 1 || int_port > 65535) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "Port value \"%s\" out of range (1-65535)", pos + 1);
            return FPX_ADDR_INVALID;
        }
        len = fpx_strlen(pos + 1);
        *port = fpx_palloc(pool, len + 1);
        fpx_memmove(*port, pos + 1, len + 1); /* Copy with '\0' character */
        end = pos - 1;
    }

    close_bracket = fpx_strchr(str, ']');

    if (*str == '[') {
        if (!close_bracket || *end != ']') {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "Missing \"]\" in the string \"%s\"", str);
            return FPX_ADDR_INVALID;
        }
        pos = str + 1;
        len = close_bracket - pos;
        rv = FPX_ADDR_IPV6;
    }
    else {
        if (close_bracket) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "Unexpected \"]\" in the string \"%s\"", str);
            return FPX_ADDR_INVALID;
        }
        pos = str;
        len = end - pos + 1;
        rv = FPX_ADDR_IPV4;
    }

    *addr = fpx_palloc(pool, len + 1);
    fpx_memmove(*addr, pos, len);
    (*addr)[len] = '\0';

    return rv;
}

fpx_uint_t
fpx_config_parse_log_level(const char *str)
{
    fpx_uint_t i;

    for (i = 0; fpx_log_prio[i] != NULL; ++i) {
        if (fpx_strcmp(str, fpx_log_prio[i]) == 0) {
            return i;
        }
    }

    return FPX_LOG_INVALID_LEVEL;
}
