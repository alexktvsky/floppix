#ifndef INCLUDED_HCNSE_COMMAND_H
#define INCLUDED_HCNSE_COMMAND_H

#include "hcnse_portable.h"
#include "hcnse_core.h"


#define HCNSE_TAKE0                    0x00000001
#define HCNSE_TAKE1                    0x00000002
#define HCNSE_TAKE2                    0x00000004
#define HCNSE_TAKE3                    0x00000008
#define HCNSE_TAKE4                    0x00000010
#define HCNSE_TAKE5                    0x00000020
#define HCNSE_TAKE6                    0x00000040
#define HCNSE_TAKE7                    0x00000080

#define HCNSE_MAX_TAKES                8

#define HCNSE_NULL_COMMAND             {NULL, 0, NULL, 0}

#if (HCNSE_POSIX)
#define HCNSE_DEFAULT_CONFIG_PATH  "/etc/hcnse/hcnse.conf"
#elif (HCNSE_WIN32)
#define HCNSE_DEFAULT_CONFIG_PATH  "C:\\hcnse\\config\\hcnse.conf";
#endif


/*
 * The command record structure. Modules can define a table of these
 * to define the directives it will implement.
 */
struct hcnse_command_s {
    /* Name of this command */
    const char *name;
    /* How many arguments takes directive */
    hcnse_bitfield_t takes;
    /* The function to be called when this directive is parsed */
    hcnse_cmd_handler_t handler;
    /* The offset of field in context structure */
    hcnse_off_t offset;
};

/*
 * This structure is passed to a command which is being invoked,
 * to carry a large variety of miscellaneous data.
 */
struct hcnse_cmd_params_s {
    /* Server being configured for */
    hcnse_server_t *server;
    /* The current command */
    hcnse_command_t *cmd;
    /* The directive specifying this command */
    hcnse_directive_t *directive;

    hcnse_config_t *config;
};

/*
 * This structure store the directives that will be active in
 * the running server.
 */
struct hcnse_directive_s {
    /* The current directive name */
    const char *name;
    /*  Number of arguments */
    int argc;
    /* The arguments for the current directive */
    char **argv;
    /* The name of the file this directive was found in */
    const char *filename;
    /* The line number the directive was on */
    hcnse_uint_t line;
};

/*
 * This structure store the config structure (list, tree, etc)
 * TODO: Abstract syntax tree
 */
struct hcnse_config_s {
    /* List of directives, nodes type is hcnse_directive_t */
    hcnse_list_t *conf_list;

    hcnse_list_t *conf_files;
};


hcnse_err_t hcnse_config_read(hcnse_config_t *config, hcnse_pool_t *pool,
    const char *filename);
hcnse_err_t hcnse_config_read_included(hcnse_config_t *config,
    hcnse_pool_t *pool, const char *path);
hcnse_err_t hcnse_config_check(hcnse_config_t *config,
    hcnse_server_t *server);
hcnse_err_t hcnse_config_process(hcnse_config_t *config,
    hcnse_server_t *server);

hcnse_err_t hcnse_handler_flag(hcnse_cmd_params_t *params, void *data,
    int argc, char **argv);
hcnse_err_t hcnse_handler_str(hcnse_cmd_params_t *params, void *data,
    int argc, char **argv);
hcnse_err_t hcnse_handler_size(hcnse_cmd_params_t *params, void *data,
    int argc, char **argv);
hcnse_err_t hcnse_handler_uint(hcnse_cmd_params_t *params, void *data,
    int argc, char **argv);

#endif /* INCLUDED_HCNSE_COMMAND_H */
