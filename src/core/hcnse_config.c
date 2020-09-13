#include "hcnse_portable.h"
#include "hcnse_core.h"


static hcnse_bitmask_t hcnse_takes[] = {
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
    const char *name, int argc, char **argv, const char *filename,
    hcnse_uint_t line)
{
    hcnse_directive_t *directive;
    size_t argv_size;


    directive = hcnse_pcalloc(pool, sizeof(hcnse_directive_t));
    if (!directive) {
        return hcnse_get_errno();
    }

    directive->name = name;
    directive->argc = argc;
    directive->filename = filename;
    directive->line = line;

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
    hcnse_file_t *file;
    const char *filename;

    const char *begin, *end;
    size_t len;

    const char *str;
    int argc;
    char *argv[HCNSE_MAX_TAKES];
    char *alloc_str;
    char c;

    hcnse_uint_t found, comment, in_directive, end_line, end_file;
    hcnse_uint_t i, line;

    file = hcnse_list_last(config->conf_files)->data;
    filename = file->name;

    begin = NULL;
    end = NULL;

    argc = 0;
    line = 1;

    found = 0;
    comment = 0;
    in_directive = 0;
    end_line = 0;
    end_file = 0;

    for (i = 0; ; ++i) {

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
                    "%s:%zu: Unexpected \"#\", probably missing terminating "
                    "character", filename, line);
                return HCNSE_ERR_CONFIG_SYNTAX;
            }
            comment = 1;
            break;

        default:

            if (!hcnse_isascii(c)) {
                hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                    "%s:%zu: Unexpected non-ascii binary data", filename, line);
                return HCNSE_ERR_CONFIG_SYNTAX;
            }

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
                str = alloc_str;
                goto next;
            }

            if (in_directive) {
                argv[argc] = alloc_str;
                argc += 1;
                if (argc >= HCNSE_MAX_TAKES) {
                    hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                        "%s:%zu: Too many arguments in directive \"%s\"",
                        filename, line, str);
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
                hcnse_config_save_directive(config, pool, str, argc, argv,
                    filename, line);
            }

            in_directive = 0;
            argc = 0;
            line += 1;
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
    for ( ; iter; iter = hcnse_list_next(iter)) {
        module = hcnse_list_data(iter);
        for (i = 0; module->cmd[i].name != NULL; ++i) {
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
    for ( ; iter; iter = hcnse_list_next(iter)) {
        module = hcnse_list_data(iter);
        for (i = 0; module->cmd[i].name != NULL; ++i) {
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

    for (i = 0; i < HCNSE_MAX_TAKES; ++i) {
        if (hcnse_bit_is_set(cmd->takes, hcnse_takes[i])) {
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

static hcnse_err_t
hcnse_config_read_included_file(hcnse_config_t *config, hcnse_pool_t *pool,
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

    err = hcnse_file_open(file, filename, HCNSE_FILE_RDONLY,
                    HCNSE_FILE_OPEN, HCNSE_FILE_DEFAULT_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_pool_cleanup_add(pool, file, hcnse_file_close);

    file_size = hcnse_file_size(file);
    if (file_size == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    file_buf = hcnse_palloc(pool, file_size + 1);
    if (!file_buf) {
        err = hcnse_get_errno();
        goto failed;
    }

    bytes_read = hcnse_file_read(file, (uint8_t *) file_buf, file_size, 0);
    if (bytes_read == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    file_buf[bytes_read] = '\0';

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
hcnse_config_read(hcnse_config_t *config, hcnse_pool_t *pool,
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

    err = hcnse_file_open(file, filename, HCNSE_FILE_RDONLY,
                    HCNSE_FILE_OPEN, HCNSE_FILE_DEFAULT_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_pool_cleanup_add(pool, file, hcnse_file_close);

    file_size = hcnse_file_size(file);
    if (file_size == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    file_buf = hcnse_palloc(pool, file_size + 1);
    if (!file_buf) {
        err = hcnse_get_errno();
        goto failed;
    }

    bytes_read = hcnse_file_read(file, (uint8_t *) file_buf, file_size, 0);
    if (bytes_read == -1) {
        err = hcnse_get_errno();
        goto failed;
    }

    file_buf[bytes_read] = '\0';

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
hcnse_config_check(hcnse_config_t *config, hcnse_server_t *server)
{
    hcnse_cmd_params_t params;
    hcnse_directive_t *directive;
    hcnse_command_t *cmd;
    hcnse_module_t *module;
    hcnse_list_node_t *iter;
    hcnse_err_t err;

    iter = hcnse_list_first(config->conf_list);

    for ( ; iter; iter = hcnse_list_next(iter)) {

        directive = hcnse_list_data(iter);

        cmd = hcnse_find_command_in_modules(server, directive->name);
        if (!cmd) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "%s:%zu: Unknown directive \"%s\"",
                directive->filename, directive->line, directive->name);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }

        err = hcnse_check_directive_arguments(directive, cmd);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "%s:%zu: Invalid number of arguments in directive \"%s\"",
                directive->filename, directive->line, directive->name);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }

        if (hcnse_strcmp(directive->name, "include") != 0) {
            continue;
        }

        module = hcnse_find_module_by_command(server, directive->name);
        if (!module) {
            /* Unreachable */
            return HCNSE_FAILED;
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
hcnse_config_process(hcnse_config_t *config, hcnse_server_t *server)
{
    hcnse_cmd_params_t params;
    hcnse_directive_t *directive;
    hcnse_command_t *cmd;
    hcnse_module_t *module;
    hcnse_list_node_t *iter;
    hcnse_err_t err;


    iter = hcnse_list_first(config->conf_list);
    for ( ; iter; iter = hcnse_list_next(iter)) {
        directive = hcnse_list_data(iter);
        cmd = hcnse_find_command_in_modules(server, directive->name);
        if (!cmd) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "%s:%zu: Unknown directive \"%s\"",
                directive->filename, directive->line, directive->name);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }

        err = hcnse_check_directive_arguments(directive, cmd);
        if (err != HCNSE_OK) {
            hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_ERR_CONFIG_SYNTAX,
                "%s:%zu: Invalid number of arguments in directive \"%s\"",
                directive->filename, directive->line, directive->name);
            return HCNSE_ERR_CONFIG_SYNTAX;
        }

        module = hcnse_find_module_by_command(server, directive->name);
        if (!module) {
            /* Unreachable */
            return HCNSE_FAILED;
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
hcnse_config_walkdir_nonwildcard(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *path)
{
    char fullpath[HCNSE_MAX_PATH_LEN];
    const char *fname;
    hcnse_dir_t dir;
    hcnse_err_t err;


    if ((err = hcnse_dir_open(&dir, path)) != HCNSE_DONE) {
        return err;
    }

    while (hcnse_dir_read(&dir) == HCNSE_OK) {

        fname = hcnse_dir_current_name(&dir);

        if ((hcnse_strcmp(fname, ".") == 0) ||
            (hcnse_strcmp(fname, "..") == 0))
        {
            continue;
        }

        hcnse_file_full_path(fullpath, path, fname);

        if (hcnse_dir_current_is_file(&dir)) {

            err = hcnse_config_read_included_file(config, pool, fullpath);
            if (err != HCNSE_OK) {
                goto failed;
            }

            continue;
        }

        err = hcnse_config_walkdir_nonwildcard(config, pool, fullpath);
        if (err != HCNSE_OK) {
            goto failed;
        }
    }

    hcnse_dir_close(&dir);

    return HCNSE_OK;

failed:
    hcnse_dir_close(&dir);
    return err;
}

#if 0
hcnse_err_t
hcnse_config_walkdir_wildcard(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *path, const char *fname)
{
    char fullpath[HCNSE_MAX_PATH_LEN];
    hcnse_dir_t dir;
    const char *pos, *dir_name;
    hcnse_err_t err;


    pos = hcnse_strchr(fname, HCNSE_PATH_SEPARATOR);
    if (pos) {
        fname = hcnse_pstrndup(pool, fname, pos - fname);
        pos += 1;
    }

    if (!hcnse_is_part_has_wildcard(fname)) {

        hcnse_file_full_path(fullpath, path, fname);

        if (!pos) {
            return hcnse_config_walkdir_nonwildcard(config, pool, fullpath);
        }
        else {
            return hcnse_config_walkdir_wildcard(config, pool, fullpath, pos);
        }
    }

    if ((err = hcnse_dir_open(&dir, path)) != HCNSE_OK) {
        return err;
    }

    while (hcnse_dir_read(&dir) == HCNSE_OK) {

        dir_name = hcnse_dir_current_name(&dir);

        if ((hcnse_strcmp(dir_name, ".") == 0) ||
            (hcnse_strcmp(dir_name, "..") == 0) ||
            !hcnse_is_glob_match(dir_name, fname))
        {
            continue;
        }

        hcnse_file_full_path(fullpath, path, dir_name);

        if (hcnse_dir_current_is_file(&dir)) {

            if (!pos) {
                err = hcnse_config_read_included_file(config, pool, fullpath);
                if (err != HCNSE_OK) {
                    goto failed;
                }
            }
            continue;
        }
        else {
            if (pos) {
                err = hcnse_config_walkdir_wildcard(config, pool, fullpath, pos);
                if (err != HCNSE_OK) {
                    goto failed;
                }
            }
        }
    }

    hcnse_dir_close(&dir);

    return HCNSE_OK;

failed:

    hcnse_dir_close(&dir);

    return err;
}
#endif

hcnse_err_t
hcnse_config_walkdir_wildcard(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *path)
{
    char fullpath[HCNSE_MAX_PATH_LEN];
    hcnse_file_info_t info;
    hcnse_glob_t glob;
    hcnse_err_t err;


    err = hcnse_glob_open(&glob, path);
    if (err != HCNSE_OK) {
        return err;
    }

    while (hcnse_glob_read(&glob, fullpath) != HCNSE_DONE) {

        if ((hcnse_strcmp(fullpath, ".") == 0) ||
            (hcnse_strcmp(fullpath, "..") == 0))
        {
            continue;
        }

        hcnse_file_info_by_path(&info, fullpath);
        if (info.type == HCNSE_FILE_TYPE_FILE) {
            err = hcnse_config_read_included_file(config, pool, fullpath);
        }
        else {
            err = hcnse_config_walkdir_nonwildcard(config, pool, fullpath);
        }
    }

    hcnse_glob_close(&glob);

    return HCNSE_OK;
}

hcnse_err_t
hcnse_config_read_included(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *path)
{
    hcnse_file_info_t info;
    hcnse_err_t err;


    err = hcnse_check_absolute_path(path);
    if (err != HCNSE_OK) {
        hcnse_log_error1(HCNSE_LOG_ERROR, err,
            "Failed to include \"%s\"", path);
        return err;
    }

    if (!hcnse_is_path_has_wildcard(path)) {

        hcnse_file_info_by_path(&info, path);

        if (info.type == HCNSE_FILE_TYPE_FILE) {
            err = hcnse_config_read_included_file(config, pool, path);
        }
        else {
            err = hcnse_config_walkdir_nonwildcard(config, pool, path);
        }
    }
    else {

        err = hcnse_config_walkdir_wildcard(config, pool, path);
    }

    return err;
}

hcnse_err_t
hcnse_handler_flag(hcnse_cmd_params_t *params, void *data, int argc,
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
hcnse_handler_str(hcnse_cmd_params_t *params, void *data, int argc,
    char **argv)
{
    char **ptr;
    (void) argc;

    ptr = (char **) (data + params->cmd->offset);

    *ptr = argv[0];

    return HCNSE_OK;
}

hcnse_err_t
hcnse_handler_size(hcnse_cmd_params_t *params, void *data, int argc,
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
hcnse_handler_uint(hcnse_cmd_params_t *params, void *data, int argc,
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
