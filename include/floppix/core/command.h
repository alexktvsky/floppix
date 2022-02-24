#ifndef FLOPPIX_CORE_COMMAND_H
#define FLOPPIX_CORE_COMMAND_H

#include "floppix/util/bitmask.h"
#include "floppix/system/os/portable.h"
#include "floppix/core/server.h"
#include "floppix/algo/list.h"

#define FPX_TAKE0     0x00000001
#define FPX_TAKE1     0x00000002
#define FPX_TAKE2     0x00000004
#define FPX_TAKE3     0x00000008
#define FPX_TAKE4     0x00000010
#define FPX_TAKE5     0x00000020
#define FPX_TAKE6     0x00000040
#define FPX_TAKE7     0x00000080

#define FPX_MAX_TAKES 8

#define FPX_NULL_COMMAND                                                       \
    {                                                                          \
        NULL, 0, NULL, 0                                                       \
    }

#if (FPX_POSIX)
#define FPX_DEFAULT_CONFIG_PATH "/etc/fpx/floppix.conf"
#elif (FPX_WIN32)
#define FPX_DEFAULT_CONFIG_PATH "C:\\fpx\\config\\floppix.conf";
#endif

typedef fpx_err_t (*fpx_cmd_handler_t)(fpx_cmd_params_t *params, void *data,
    int argc, char **argv);

/*
 * The command record structure. Modules can define a table of these
 * to define the directives it will implement.
 */
struct fpx_command_s {
    /* Name of this command */
    const char *name;
    /* How many arguments takes directive */
    fpx_bitmask_t takes;
    /* The function to be called when this directive is parsed */
    fpx_cmd_handler_t handler;
    /* The offset of field in context structure */
    size_t offset;
};

/*
 * This structure is passed to a command which is being invoked,
 * to carry a large variety of miscellaneous data.
 */
struct fpx_cmd_params_s {
    /* Server being configured for */
    fpx_server_t *server;
    /* The current command */
    fpx_command_t *cmd;
    /* The directive specifying this command */
    fpx_directive_t *directive;

    fpx_config_t *config;
};

/*
 * This structure store the directives that will be active in
 * the running server.
 */
struct fpx_directive_s {
    /* The current directive name */
    const char *name;
    /*  Number of arguments */
    int argc;
    /* The arguments for the current directive */
    char **argv;
    /* The name of the file this directive was found in */
    const char *filename;
    /* The line number the directive was on */
    uint line;
    /* List node */
    fpx_list_node_t list_node;
};

/*
 * This structure store the config structure (list, tree, etc)
 * TODO: Abstract syntax tree
 */
struct fpx_config_s {
    /* List of directives, nodes type is fpx_directive_t */
    fpx_list_t conf_list;
    /* List of previous configs */
    fpx_list_t conf_files;
};

fpx_err_t fpx_config_read(fpx_config_t *config, fpx_pool_t *pool,
    const char *filename);
fpx_err_t fpx_config_read_included(fpx_config_t *config, fpx_pool_t *pool,
    const char *path);
fpx_err_t fpx_config_check(fpx_config_t *config, fpx_server_t *server);
fpx_err_t fpx_config_process(fpx_config_t *config, fpx_server_t *server);

fpx_err_t fpx_handler_flag(fpx_cmd_params_t *params, void *data, int argc,
    char **argv);
fpx_err_t fpx_handler_str(fpx_cmd_params_t *params, void *data, int argc,
    char **argv);
fpx_err_t fpx_handler_size(fpx_cmd_params_t *params, void *data, int argc,
    char **argv);
fpx_err_t fpx_handler_uint(fpx_cmd_params_t *params, void *data, int argc,
    char **argv);

#endif /* FLOPPIX_CORE_COMMAND_H */
