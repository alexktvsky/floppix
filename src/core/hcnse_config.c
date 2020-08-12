#include "hcnse_portable.h"
#include "hcnse_core.h"


static hcnse_flag_t hcnse_takes[] = {
    HCNSE_TAKE0,
    HCNSE_TAKE1,
    HCNSE_TAKE2,
    HCNSE_TAKE3,
    HCNSE_TAKE4,
    HCNSE_TAKE5,
    HCNSE_TAKE6,
    HCNSE_TAKE7
};


static hcnse_err_t
hcnse_config_save_directive(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *name, int argc, char **argv)
{
    hcnse_directive_t *directive;
    size_t argv_size;


    directive = hcnse_pcalloc(pool, sizeof(hcnse_directive_t));
    if (!directive) {
        return hcnse_get_errno();
    }

    directive->name = name;
    directive->argc = argc;

    argv_size = argc * sizeof(void *);

    directive->argv = hcnse_palloc(pool, argv_size);
    if (!directive->argv) {
        return hcnse_get_errno();
    }
    hcnse_memmove(directive->argv, argv, argv_size);

    hcnse_list_push_back(config->conf_list, directive);

    return HCNSE_OK;
}

static hcnse_err_t
hcnse_config_parse(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *file_buf)
{
    const char *begin, *end;
    size_t len;

    const char *name;
    int argc;
    char *argv[HCNSE_MAX_TAKES];
    char *alloc_str;
    char c;

    hcnse_uint_t found, comment, in_directive, end_line, end_file;
    hcnse_uint_t i;

    begin = NULL;
    end = NULL;

    argc = 0;

    found = 0;
    comment = 0;
    in_directive = 0;
    end_line = 0;
    end_file = 0;

    for (i = 0; ; i++) {

        c = file_buf[i];

        end_line = 0;

        switch (c) {
        case HCNSE_NULL:
            end_file = 1;
            break;

        case HCNSE_TAB:
        case HCNSE_SPACE:
            if (begin != NULL) {
                end = &file_buf[i];
                found = 1;
            }
            break;

        case HCNSE_LF:
        case HCNSE_CR:
            end_line = 1;
            comment = 0;

            if (begin != NULL) {
                end = &file_buf[i];
                found = 1;
            }
            break;

        case '#':
            if (begin != NULL) {
                hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                    "Unexpected \"#\", probably missing terminating character");
                return 1;
            }
            comment = 1;
            break;

        default:
            /* Begin of word outside comment section */
            if (begin == NULL && !comment) {
                begin = &file_buf[i];
            }
        }

        if (found) {

            len = (size_t) (end - begin);

            alloc_str = hcnse_pstrndup(pool, begin, len);
            if (!alloc_str) {
                return hcnse_get_errno();
            }

            if (!in_directive) {
                in_directive = 1;
                name = alloc_str;
                goto next;
            }

            if (in_directive) {
                argv[argc] = alloc_str;
                argc += 1;
                if (argc >= HCNSE_MAX_TAKES) {
                    hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                        "Too many arguments in directive \"%s\"", name);
                    return HCNSE_ERR_CONFIG_SYNTAX;
                }
            }
next:
            begin = NULL;
            end = NULL;
            found = 0;
        }

        if (end_line) {

            if (in_directive) {
                hcnse_config_save_directive(config, pool, name, argc, argv);
            }

            in_directive = 0;
            argc = 0;
        }

        if (end_file) {
            return HCNSE_OK;
        }

        continue;
    }

    return HCNSE_OK;
}

static hcnse_command_t *
hcnse_find_command_in_modules(hcnse_server_t *server, const char *cmd_name)
{
    hcnse_list_node_t *iter;
    hcnse_module_t *module;
    hcnse_uint_t i;

    iter = hcnse_list_first(server->modules);
    for ( ; iter; iter = iter->next) {
        module = iter->data;
        for (i = 0; module->cmd[i].name != NULL; i++) {
            if (hcnse_strcmp(module->cmd[i].name, cmd_name) == 0) {
                return &(module->cmd[i]);
            }
        }
    }
    return NULL;
}

static hcnse_module_t *
hcnse_find_module_by_command(hcnse_server_t *server, const char *cmd_name)
{
    hcnse_list_node_t *iter;
    hcnse_module_t *module;
    hcnse_uint_t i;

    iter = hcnse_list_first(server->modules);
    for ( ; iter; iter = iter->next) {
        module = iter->data;
        for (i = 0; module->cmd[i].name != NULL; i++) {
            if (hcnse_strcmp(module->cmd[i].name, cmd_name) == 0) {
                return module;
            }
        }
    }
    return NULL;
}

static hcnse_err_t
hcnse_check_directive_arguments(hcnse_directive_t *directive,
    hcnse_command_t *cmd)
{
    hcnse_uint_t min_takes, max_takes, i, argc;


    min_takes = 0;
    max_takes = 0;

    argc = directive->argc;

    for (i = 0; i < HCNSE_MAX_TAKES; i++) {
        if (hcnse_flag_is_set(cmd->takes, hcnse_takes[i])) {
            if (min_takes == 0) {
                min_takes = i;
            }
            if (max_takes < i) {
                max_takes = i;
            } 
        }
    }

    if (!(argc == min_takes || (argc <= max_takes && argc >= min_takes))) {
        return HCNSE_ERR_CONFIG_SYNTAX;
    }

    return HCNSE_OK;
}


hcnse_err_t
hcnse_check_config(hcnse_config_t *config, hcnse_server_t *server)
{
    hcnse_directive_t *directive;
    hcnse_command_t *cmd;
    hcnse_list_node_t *iter;
    hcnse_err_t err;


    iter = hcnse_list_first(config->conf_list);

    for ( ; iter; iter = iter->next) {

        directive = iter->data;

        cmd = hcnse_find_command_in_modules(server, directive->name);
        if (!cmd) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Unknown directive \"%s\"", directive->name);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }

        err = hcnse_check_directive_arguments(directive, cmd);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Invalid number of arguments in directive \"%s\"",
                directive->name);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_process_config(hcnse_config_t *config, hcnse_server_t *server)
{
    hcnse_cmd_params_t params;

    hcnse_directive_t *directive;
    hcnse_command_t *cmd;
    hcnse_module_t *module;
    hcnse_list_node_t *iter;
    hcnse_err_t err;


    iter = hcnse_list_first(config->conf_list);
    for ( ; iter; iter = iter->next) {
        directive = iter->data;
        cmd = hcnse_find_command_in_modules(server, directive->name);
        if (!cmd) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Unknown directive \"%s\"", directive->name);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }

        err = hcnse_check_directive_arguments(directive, cmd);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "Invalid number of arguments in directive \"%s\"",
                directive->name);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }

        module = hcnse_find_module_by_command(server, directive->name);
        if (!module) {
            return HCNSE_FAILED; /* Not possible */
        }

        params.server = server;
        params.cmd = cmd;
        params.directive = directive;
        params.config = config;

        err = cmd->handler(&params, module->cntx, directive->argc,
            directive->argv);
        if (err != HCNSE_OK) {
            return err;
        }
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_read_config(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *filename)
{
    hcnse_list_t *conf_list;
    hcnse_list_t *conf_files;
    hcnse_file_t *file;
    char *file_buf;
    ssize_t file_size;
    ssize_t bytes_read;

    hcnse_err_t err;


    file = hcnse_palloc(pool, sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_file_init(file, filename, HCNSE_FILE_RDONLY,
                    HCNSE_FILE_OPEN, HCNSE_FILE_DEFAULT_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_pool_cleanup_add(pool, file, hcnse_file_fini);

    file_size = hcnse_file_size(file);
    if (file_size == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    file_buf = hcnse_palloc(pool, file_size);
    if (!file_buf) {
        err = hcnse_get_errno();
        goto failed;
    }

    bytes_read = hcnse_file_read(file, (uint8_t *) file_buf, file_size, 0);
    if (bytes_read == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    conf_list = hcnse_list_create(pool);
    if (!conf_list) {
        err = hcnse_get_errno();
        goto failed;
    }

    conf_files = hcnse_list_create(pool);
    if (!conf_files) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_list_push_back(conf_files, file);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_memzero(config, sizeof(hcnse_config_t));

    config->conf_list = conf_list;
    config->conf_files = conf_files;

    err = hcnse_config_parse(config, pool, file_buf);
    if (err != HCNSE_OK) {
        goto failed;
    }

    return HCNSE_OK;

failed:

    return err;
}

hcnse_err_t
hcnse_read_included_config(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *filename)
{
    hcnse_file_t *file;
    char *file_buf;
    ssize_t file_size;
    ssize_t bytes_read;

    hcnse_err_t err;

    file = hcnse_palloc(pool, sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_file_init(file, filename, HCNSE_FILE_RDONLY,
                    HCNSE_FILE_OPEN, HCNSE_FILE_DEFAULT_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_pool_cleanup_add(pool, file, hcnse_file_fini);

    file_size = hcnse_file_size(file);
    if (file_size == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    file_buf = hcnse_palloc(pool, file_size);
    if (!file_buf) {
        err = hcnse_get_errno();
        goto failed;
    }

    bytes_read = hcnse_file_read(file, (uint8_t *) file_buf, file_size, 0);
    if (bytes_read == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_list_push_back(config->conf_files, file);
    if (err != HCNSE_OK) {
        goto failed;
    }

    err = hcnse_config_parse(config, pool, file_buf);
    if (err != HCNSE_OK) {
        goto failed;
    }

    return HCNSE_OK;

failed:

    return err;
}

hcnse_err_t
hcnse_handler_set_flag(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    hcnse_flag_t *ptr;
    (void) argc;

    ptr = (hcnse_flag_t *) (data + params->cmd->offset);

    if (hcnse_strcasecmp(argv[0], "on") == 0) {
        *ptr = 1;
    }
    else if (hcnse_strcasecmp(argv[0], "off") == 0) {
        *ptr = 0;
    }
    else {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
            "Invalid value \"%s\" in directive \"%s\"", argv[0],
            params->directive->name);
        return HCNSE_ERR_CONFIG_SYNTAX;
    }

    return HCNSE_OK;
}

hcnse_err_t
hcnse_handler_set_str(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    char **ptr;
    (void) argc;

    ptr = (char **) (data + params->cmd->offset);

    *ptr = argv[0];

    return HCNSE_OK;
}

hcnse_err_t
hcnse_handler_set_size(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    ssize_t *ptr, n;
    (void) argc;

    ptr = (ssize_t *) (data + params->cmd->offset);

    n = hcnse_config_parse_size(argv[0]);
    if (n == -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
            "Invalid value \"%s\" in directive \"%s\"", argv[0],
            params->directive->name);
        return HCNSE_ERR_CONFIG_SYNTAX;
    }

    *ptr = n;

    return HCNSE_OK;
}

hcnse_err_t
hcnse_handler_set_uint(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    hcnse_int_t *ptr, n;
    (void) argc;

    ptr = (hcnse_int_t *) (data + params->cmd->offset);

    n = hcnse_config_parse_size(argv[0]);
    if (n == -1) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
            "Invalid value \"%s\" in directive \"%s\"", argv[0],
            params->directive->name);
        return HCNSE_ERR_CONFIG_SYNTAX;
    }

    *ptr = n;

    return HCNSE_OK;
}
