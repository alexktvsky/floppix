#include "fpx.system.memory.h"
#include "fpx.util.string.h"
#include "fpx.core.command.h"
#include "fpx.core.module.h"
#include "fpx.core.parse.h"

static fpx_bitmask_t fpx_takes[] = {
    FPX_TAKE0,
    FPX_TAKE1,
    FPX_TAKE2,
    FPX_TAKE3,
    FPX_TAKE4,
    FPX_TAKE5,
    FPX_TAKE6,
    FPX_TAKE7,
};

static fpx_err_t
fpx_config_save_directive(fpx_config_t *config, fpx_pool_t *pool,
    const char *name, fpx_int_t argc, char **argv, const char *filename,
    fpx_uint_t line)
{
    fpx_directive_t *directive;
    fpx_size_t argv_size;

    directive = fpx_pcalloc(pool, sizeof(fpx_directive_t));
    if (!directive) {
        return fpx_get_errno();
    }

    directive->name = name;
    directive->argc = argc;
    directive->filename = filename;
    directive->line = line;

    argv_size = argc * sizeof(void *);

    directive->argv = fpx_palloc(pool, argv_size);
    if (!directive->argv) {
        return fpx_get_errno();
    }
    fpx_memmove(directive->argv, argv, argv_size);

    fpx_list_push_back(config->conf_list, directive);

    return FPX_OK;
}

static fpx_err_t
fpx_config_parse(fpx_config_t *config, fpx_pool_t *pool, const char *file_buf)
{
    fpx_file_t *file;
    const char *filename;

    const char *begin = NULL;
    const char *end = NULL;
    fpx_size_t len;

    const char *str;
    fpx_int_t argc = 0;
    char *argv[FPX_MAX_TAKES];
    char *alloc_str;
    char c;

    fpx_bool_t found = false;
    fpx_bool_t comment = false;
    fpx_bool_t in_directive = false;
    fpx_bool_t end_line = false;
    fpx_bool_t end_file = false;

    fpx_uint_t i;
    fpx_uint_t line = 1;

    file = config->conf_files->tail->data;
    filename = file->name;

    for (i = 0;; ++i) {

        c = file_buf[i];

        end_line = false;

        switch (c) {
        case FPX_NULL:
            end_file = true;
            break;

        case FPX_TAB:
        case FPX_SPACE:
            if (begin != NULL) {
                end = &file_buf[i];
                found = true;
            }
            break;

        case FPX_LF:
        case FPX_CR:
            end_line = true;
            comment = false;

            if (begin != NULL) {
                end = &file_buf[i];
                found = true;
            }
            break;

        case '#':
            if (begin != NULL) {
                fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                    "%s:%zu: Unexpected \"#\", probably missing terminating "
                    "character",
                    filename, line);
                return FPX_ERR_CONFIG_SYNTAX;
            }
            comment = true;
            break;

        default:

            if (!fpx_isascii(c)) {
                fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                    "%s:%zu: Unexpected non-ascii binary data", filename, line);
                return FPX_ERR_CONFIG_SYNTAX;
            }

            /* Begin of word outside comment section */
            if (begin == NULL && !comment) {
                begin = &file_buf[i];
            }
        }

        if (found) {

            len = (fpx_size_t) (end - begin);

            alloc_str = fpx_pstrndup(pool, begin, len);
            if (!alloc_str) {
                return fpx_get_errno();
            }

            if (!in_directive) {
                in_directive = true;
                str = alloc_str;
                goto next;
            }

            if (in_directive) {
                argv[argc] = alloc_str;
                argc += 1;
                if (argc >= FPX_MAX_TAKES) {
                    fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                        "%s:%zu: Too many arguments in directive \"%s\"",
                        filename, line, str);
                    return FPX_ERR_CONFIG_SYNTAX;
                }
            }
next:
            begin = NULL;
            end = NULL;
            found = false;
        }

        if (end_line) {

            if (in_directive) {
                fpx_config_save_directive(config, pool, str, argc, argv,
                    filename, line);
            }

            in_directive = false;
            argc = false;
            line += 1;
        }

        if (end_file) {
            return FPX_OK;
        }

        continue;
    }

    return FPX_OK;
}

static fpx_command_t *
fpx_find_command_in_modules(fpx_server_t *server, const char *cmd_name)
{
    fpx_list_node_t *node;
    fpx_module_t *module;
    fpx_uint_t i;

    node = server->modules->head;
    for (; node; node = node->next) {
        module = (fpx_module_t *) node->data;
        for (i = 0; module->cmd[i].name != NULL; ++i) {
            if (fpx_strcmp(module->cmd[i].name, cmd_name) == 0) {
                return &(module->cmd[i]);
            }
        }
    }
    return NULL;
}

static fpx_module_t *
fpx_find_module_by_command(fpx_server_t *server, const char *cmd_name)
{
    fpx_list_node_t *node;
    fpx_module_t *module;
    fpx_uint_t i;

    node = server->modules->head;
    for (; node; node = node->next) {
        module = (fpx_module_t *) node->data;
        for (i = 0; module->cmd[i].name != NULL; ++i) {
            if (fpx_strcmp(module->cmd[i].name, cmd_name) == 0) {
                return module;
            }
        }
    }
    return NULL;
}

static fpx_err_t
fpx_check_directive_arguments(fpx_directive_t *directive, fpx_command_t *cmd)
{
    fpx_uint_t min_takes, max_takes, i, argc;

    min_takes = 0;
    max_takes = 0;

    argc = directive->argc;

    for (i = 0; i < FPX_MAX_TAKES; ++i) {
        if (fpx_bit_is_set(cmd->takes, fpx_takes[i])) {
            if (min_takes == 0) {
                min_takes = i;
            }
            if (max_takes < i) {
                max_takes = i;
            }
        }
    }

    if (!(argc == min_takes || (argc <= max_takes && argc >= min_takes))) {
        return FPX_ERR_CONFIG_SYNTAX;
    }

    return FPX_OK;
}

static fpx_err_t
fpx_config_read_included_file(fpx_config_t *config, fpx_pool_t *pool,
    const char *filename)
{
    fpx_file_t *file;
    char *file_buf;
    fpx_ssize_t file_size;
    fpx_ssize_t bytes_read;

    fpx_err_t err;

    file = fpx_palloc(pool, sizeof(fpx_file_t));
    if (!file) {
        err = fpx_get_errno();
        goto failed;
    }

    err = fpx_file_open(file, filename, FPX_FILE_RDONLY, FPX_FILE_OPEN,
        FPX_FILE_DEFAULT_ACCESS);
    if (err != FPX_OK) {
        goto failed;
    }

    fpx_pool_cleanup_add(pool, file, fpx_file_close);

    file_size = fpx_file_size(file);
    if (file_size == -1) {
        err = fpx_get_errno();
        goto failed;
    }

    file_buf = fpx_palloc(pool, file_size + 1);
    if (!file_buf) {
        err = fpx_get_errno();
        goto failed;
    }

    bytes_read = fpx_file_read(file, (uint8_t *) file_buf, file_size, 0);
    if (bytes_read == -1) {
        err = fpx_get_errno();
        goto failed;
    }

    file_buf[bytes_read] = '\0';

    err = fpx_list_push_back(config->conf_files, file);
    if (err != FPX_OK) {
        goto failed;
    }

    err = fpx_config_parse(config, pool, file_buf);
    if (err != FPX_OK) {
        goto failed;
    }

    return FPX_OK;

failed:

    return err;
}

fpx_err_t
fpx_config_read(fpx_config_t *config, fpx_pool_t *pool, const char *filename)
{
    fpx_list_t *conf_list;
    fpx_list_t *conf_files;
    fpx_file_t *file;
    char *file_buf;
    fpx_ssize_t file_size;
    fpx_ssize_t bytes_read;

    fpx_err_t err;

    file = fpx_palloc(pool, sizeof(fpx_file_t));
    if (!file) {
        err = fpx_get_errno();
        goto failed;
    }

    err = fpx_file_open(file, filename, FPX_FILE_RDONLY, FPX_FILE_OPEN,
        FPX_FILE_DEFAULT_ACCESS);
    if (err != FPX_OK) {
        goto failed;
    }

    fpx_pool_cleanup_add(pool, file, fpx_file_close);

    file_size = fpx_file_size(file);
    if (file_size == -1) {
        err = fpx_get_errno();
        goto failed;
    }

    file_buf = fpx_palloc(pool, file_size + 1);
    if (!file_buf) {
        err = fpx_get_errno();
        goto failed;
    }

    bytes_read = fpx_file_read(file, (uint8_t *) file_buf, file_size, 0);
    if (bytes_read == -1) {
        err = fpx_get_errno();
        goto failed;
    }

    file_buf[bytes_read] = '\0';

    if ((err = fpx_list_init(&conf_list, pool)) != FPX_OK) {
        goto failed;
    }

    if ((err = fpx_list_init(&conf_files, pool)) != FPX_OK) {
        goto failed;
    }

    err = fpx_list_push_back(conf_files, file);
    if (err != FPX_OK) {
        goto failed;
    }

    fpx_memzero(config, sizeof(fpx_config_t));

    config->conf_list = conf_list;
    config->conf_files = conf_files;

    err = fpx_config_parse(config, pool, file_buf);
    if (err != FPX_OK) {
        goto failed;
    }

    return FPX_OK;

failed:

    return err;
}

fpx_err_t
fpx_config_check(fpx_config_t *config, fpx_server_t *server)
{
    fpx_cmd_params_t params;
    fpx_directive_t *directive;
    fpx_command_t *cmd;
    fpx_module_t *module;
    fpx_list_node_t *node;
    fpx_err_t err;

    node = config->conf_list->head;

    for (; node; node = node->next) {

        directive = (fpx_directive_t *) node->data;

        cmd = fpx_find_command_in_modules(server, directive->name);
        if (!cmd) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "%s:%zu: Unknown directive \"%s\"", directive->filename,
                directive->line, directive->name);
            return FPX_ERR_CONFIG_SYNTAX;
        }

        err = fpx_check_directive_arguments(directive, cmd);
        if (err != FPX_OK) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "%s:%zu: Invalid number of arguments in directive \"%s\"",
                directive->filename, directive->line, directive->name);
            return FPX_ERR_CONFIG_SYNTAX;
        }

        if (fpx_strcmp(directive->name, "include") != 0) {
            continue;
        }

        module = fpx_find_module_by_command(server, directive->name);
        if (!module) {
            /* Unreachable */
            return FPX_FAILED;
        }

        params.server = server;
        params.cmd = cmd;
        params.directive = directive;
        params.config = config;

        err = cmd->handler(&params, module->cntx, directive->argc,
            directive->argv);
        if (err != FPX_OK) {
            return err;
        }
    }

    return FPX_OK;
}

fpx_err_t
fpx_config_process(fpx_config_t *config, fpx_server_t *server)
{
    fpx_cmd_params_t params;
    fpx_directive_t *directive;
    fpx_command_t *cmd;
    fpx_module_t *module;
    fpx_list_node_t *node;
    fpx_err_t err;

    node = config->conf_list->head;
    for (; node; node = node->next) {
        directive = (fpx_directive_t *) node->data;
        cmd = fpx_find_command_in_modules(server, directive->name);
        if (!cmd) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "%s:%zu: Unknown directive \"%s\"", directive->filename,
                directive->line, directive->name);
            return FPX_ERR_CONFIG_SYNTAX;
        }

        err = fpx_check_directive_arguments(directive, cmd);
        if (err != FPX_OK) {
            fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
                "%s:%zu: Invalid number of arguments in directive \"%s\"",
                directive->filename, directive->line, directive->name);
            return FPX_ERR_CONFIG_SYNTAX;
        }

        module = fpx_find_module_by_command(server, directive->name);
        if (!module) {
            /* Unreachable */
            return FPX_FAILED;
        }

        params.server = server;
        params.cmd = cmd;
        params.directive = directive;
        params.config = config;

        err = cmd->handler(&params, module->cntx, directive->argc,
            directive->argv);
        if (err != FPX_OK) {
            return err;
        }
    }

    return FPX_OK;
}

fpx_err_t
fpx_config_walkdir_nonwildcard(fpx_config_t *config, fpx_pool_t *pool,
    const char *path)
{
    char fullpath[FPX_MAX_PATH_LEN];
    const char *fname;
    fpx_dir_t dir;
    fpx_err_t err;

    if ((err = fpx_dir_open(&dir, path)) != FPX_DONE) {
        return err;
    }

    while (fpx_dir_read(&dir) == FPX_OK) {

        fname = fpx_dir_current_name(&dir);

        if ((fpx_strcmp(fname, ".") == 0) || (fpx_strcmp(fname, "..") == 0)) {
            continue;
        }

        fpx_file_full_path(fullpath, path, fname);

        if (fpx_dir_current_is_file(&dir)) {

            err = fpx_config_read_included_file(config, pool, fullpath);
            if (err != FPX_OK) {
                goto failed;
            }

            continue;
        }

        err = fpx_config_walkdir_nonwildcard(config, pool, fullpath);
        if (err != FPX_OK) {
            goto failed;
        }
    }

    fpx_dir_close(&dir);

    return FPX_OK;

failed:
    fpx_dir_close(&dir);
    return err;
}

#if 0
fpx_err_t
fpx_config_walkdir_wildcard(fpx_config_t *config, fpx_pool_t *pool,
    const char *path, const char *fname)
{
    char fullpath[FPX_MAX_PATH_LEN];
    fpx_dir_t dir;
    const char *pos, *dir_name;
    fpx_err_t err;


    pos = fpx_strchr(fname, FPX_PATH_SEPARATOR);
    if (pos) {
        fname = fpx_pstrndup(pool, fname, pos - fname);
        pos += 1;
    }

    if (!fpx_is_part_has_wildcard(fname)) {

        fpx_file_full_path(fullpath, path, fname);

        if (!pos) {
            return fpx_config_walkdir_nonwildcard(config, pool, fullpath);
        }
        else {
            return fpx_config_walkdir_wildcard(config, pool, fullpath, pos);
        }
    }

    if ((err = fpx_dir_open(&dir, path)) != FPX_OK) {
        return err;
    }

    while (fpx_dir_read(&dir) == FPX_OK) {

        dir_name = fpx_dir_current_name(&dir);

        if ((fpx_strcmp(dir_name, ".") == 0) ||
            (fpx_strcmp(dir_name, "..") == 0) ||
            !fpx_is_glob_match(dir_name, fname))
        {
            continue;
        }

        fpx_file_full_path(fullpath, path, dir_name);

        if (fpx_dir_current_is_file(&dir)) {

            if (!pos) {
                err = fpx_config_read_included_file(config, pool, fullpath);
                if (err != FPX_OK) {
                    goto failed;
                }
            }
            continue;
        }
        else {
            if (pos) {
                err = fpx_config_walkdir_wildcard(config, pool, fullpath, pos);
                if (err != FPX_OK) {
                    goto failed;
                }
            }
        }
    }

    fpx_dir_close(&dir);

    return FPX_OK;

failed:

    fpx_dir_close(&dir);

    return err;
}
#endif

fpx_err_t
fpx_config_walkdir_wildcard(fpx_config_t *config, fpx_pool_t *pool,
    const char *path)
{
    char fullpath[FPX_MAX_PATH_LEN];
    fpx_file_info_t info;
    fpx_glob_t glob;
    fpx_err_t err;

    err = fpx_glob_open(&glob, path);
    if (err != FPX_OK) {
        return err;
    }

    while (fpx_glob_read(&glob, fullpath) != FPX_DONE) {

        if ((fpx_strcmp(fullpath, ".") == 0)
            || (fpx_strcmp(fullpath, "..") == 0)) {
            continue;
        }

        fpx_file_info_by_path(&info, fullpath);
        if (info.type == FPX_FILE_TYPE_FILE) {
            err = fpx_config_read_included_file(config, pool, fullpath);
        }
        else {
            err = fpx_config_walkdir_nonwildcard(config, pool, fullpath);
        }
    }

    fpx_glob_close(&glob);

    return FPX_OK;
}

fpx_err_t
fpx_config_read_included(fpx_config_t *config, fpx_pool_t *pool,
    const char *path)
{
    fpx_file_info_t info;
    fpx_err_t err;

    err = fpx_check_absolute_path(path);
    if (err != FPX_OK) {
        fpx_log_error1(FPX_LOG_ERROR, err, "Failed to include \"%s\"", path);
        return err;
    }

    if (!fpx_is_path_has_wildcard(path)) {

        fpx_file_info_by_path(&info, path);

        if (info.type == FPX_FILE_TYPE_FILE) {
            err = fpx_config_read_included_file(config, pool, path);
        }
        else {
            err = fpx_config_walkdir_nonwildcard(config, pool, path);
        }
    }
    else {

        err = fpx_config_walkdir_wildcard(config, pool, path);
    }

    return err;
}

fpx_err_t
fpx_handler_flag(fpx_cmd_params_t *params, void *data, fpx_int_t argc,
    char **argv)
{
    fpx_bool_t *ptr;
    (void) argc;

    ptr = (fpx_bool_t *) (((uint8_t *) data) + params->cmd->offset);

    if (fpx_strcasecmp(argv[0], "on") == 0) {
        *ptr = 1;
    }
    else if (fpx_strcasecmp(argv[0], "off") == 0) {
        *ptr = 0;
    }
    else {
        fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
            "Invalid value \"%s\" in directive \"%s\"", argv[0],
            params->directive->name);
        return FPX_ERR_CONFIG_SYNTAX;
    }

    return FPX_OK;
}

fpx_err_t
fpx_handler_str(fpx_cmd_params_t *params, void *data, fpx_int_t argc,
    char **argv)
{
    char **ptr;
    (void) argc;

    ptr = (char **) (((uint8_t *) data) + params->cmd->offset);

    *ptr = argv[0];

    return FPX_OK;
}

fpx_err_t
fpx_handler_size(fpx_cmd_params_t *params, void *data, fpx_int_t argc,
    char **argv)
{
    fpx_ssize_t *ptr, n;
    (void) argc;

    ptr = (fpx_ssize_t *) (((uint8_t *) data) + params->cmd->offset);

    n = fpx_config_parse_size(argv[0]);
    if (n == -1) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
            "Invalid value \"%s\" in directive \"%s\"", argv[0],
            params->directive->name);
        return FPX_ERR_CONFIG_SYNTAX;
    }

    *ptr = n;

    return FPX_OK;
}

fpx_err_t
fpx_handler_uint(fpx_cmd_params_t *params, void *data, fpx_int_t argc,
    char **argv)
{
    fpx_int_t *ptr, n;
    (void) argc;

    ptr = (fpx_int_t *) (((uint8_t *) data) + params->cmd->offset);

    n = fpx_config_parse_size(argv[0]);
    if (n == -1) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_ERR_CONFIG_SYNTAX,
            "Invalid value \"%s\" in directive \"%s\"", argv[0],
            params->directive->name);
        return FPX_ERR_CONFIG_SYNTAX;
    }

    *ptr = n;

    return FPX_OK;
}
