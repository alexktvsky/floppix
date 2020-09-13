#include "hcnse_portable.h"
#include "hcnse_core.h"


static const char *config_fname = HCNSE_DEFAULT_CONFIG_PATH;
static hcnse_uint_t show_version;
static hcnse_uint_t show_help;
static hcnse_uint_t test_config;


static hcnse_err_t
hcnse_parse_argv(hcnse_uint_t argc, const char *const *argv)
{
    hcnse_uint_t i;
    hcnse_uint_t long_argv;
    const char *p;

    for (i = 1; i < argc; ++i) {

        p = argv[i];
        if (*p++ != '-') {
            return HCNSE_FAILED;
        }

        while (*p) {

            long_argv = 0;

            switch (*p++) {
            case 'v':
                show_version = 1;
                break;

            case 'h':
                show_help = 1;
                break;

            case 't':
                test_config = 1;
                break;

            case 'c':
                if (argv[++i]) {
                    config_fname = argv[i];
                }
                else {
                    return HCNSE_FAILED;
                }
                break;

            case '-':
                long_argv = 1;
                break;

            default:
                return HCNSE_FAILED;
            }

            if (!long_argv) {
                continue;
            }

            if (hcnse_strcmp(p, "version") == 0) {
                show_version = 1;
                p += sizeof("version") - 1;
                continue;
            }

            if (hcnse_strcmp(p, "help") == 0) {
                show_help = 1;
                p += sizeof("help") - 1;
                continue;
            }

            if (hcnse_strcmp(p, "test-config") == 0) {
                test_config = 1;
                p += sizeof("test-config") - 1;
                continue;
            }

            if (hcnse_strcmp(p, "config-file") == 0) {
                if (argv[++i]) {
                    config_fname = argv[i];
                    p += sizeof("config-file") - 1;
                    continue;
                }
            }

            return HCNSE_FAILED;
        }
    }

    return HCNSE_OK;
}

static void
hcnse_show_version_info(void)
{
    hcnse_log_stdout(HCNSE_OK, "HCNSE %s %s", HCNSE_VERSION_STR, HCNSE_BUILD_DATE);
    hcnse_log_stdout(HCNSE_OK, "Target system: %s %d-bit",
        HCNSE_SYSTEM_NAME, HCNSE_PTR_WIDTH);
#ifdef HCNSE_COMPILER
    hcnse_log_stdout(HCNSE_OK, "Built by %s", HCNSE_COMPILER);
#endif
}

static void
hcnse_show_help_info(void)
{
    hcnse_log_stdout(HCNSE_OK,
        "Usage: hcnse [options...] [argments...]\n"
        "HCNSE version %s %s\n\n"
        "Options:\n"
        "  -h, --help                     Displays this message.\n"
        "  -v, --version                  Displays version information.\n"
        "  -t, --test                     Test configuration and exit.\n"
        "  -c, --config-file <file>       Specify configuration file.",
        HCNSE_VERSION_STR, HCNSE_BUILD_DATE);
}

static void
hcnse_save_argv(hcnse_server_t *server, int argc, const char *const *argv)
{
    server->argc = argc;
    server->argv = argv;
}

int
main(int argc, const char *const *argv)
{
    hcnse_server_t *server;
    hcnse_pool_t *pool;
    hcnse_pool_t *ptemp; /* Pool for temporary config stuff */
    hcnse_config_t *config;

    hcnse_list_t *modules;
    hcnse_err_t err;


    if ((err = hcnse_parse_argv(argc, argv)) != HCNSE_OK) {
        hcnse_log_stderr(0, "Invalid input parameters");
        hcnse_show_help_info();
        goto failed;
    }

    if (show_version) {
        hcnse_show_version_info();
        return 0;
    }
    if (show_help) {
        hcnse_show_help_info();
        return 0;
    }

#if (HCNSE_WIN32)
    if ((err = hcnse_winsock_init_v22()) != HCNSE_OK) {
        hcnse_log_stderr(0, "Failed to initialize Winsock 2.2");
        goto failed;
    }
#endif

    hcnse_assert(pool = hcnse_pool_create(0, NULL));
    hcnse_assert(ptemp = hcnse_pool_create(0, NULL));

    hcnse_assert(server = hcnse_pcalloc(pool, sizeof(hcnse_server_t)));

    hcnse_save_argv(server, argc, argv);

    hcnse_assert(modules = hcnse_list_create(pool));
    hcnse_assert(config = hcnse_palloc(ptemp, sizeof(hcnse_config_t)));

    if ((err = hcnse_config_read(config, ptemp, config_fname)) != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to read config file");
        goto failed;
    }

    server->pool = pool;
    server->config = config;
    server->modules = modules;

    if ((err = hcnse_setup_prelinked_modules(server)) != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to setup prelinked modules");
        goto failed;
    }

    if ((err = hcnse_preinit_modules(server)) != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to preinit modules");
        goto failed;
    }

    if (test_config) {
        if (hcnse_config_check(config, server) == HCNSE_OK) {
            hcnse_log_stderr(HCNSE_OK, "Configuration check succeeded");
        }
        else {
            hcnse_log_stderr(HCNSE_OK, "Configuration check failed");
        }
        hcnse_pool_destroy(pool);
        return 0;
    }

    if ((err = hcnse_config_process(config, server)) != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to process config");
        goto failed;
    }

    if ((err = hcnse_init_modules(server)) != HCNSE_OK) {
        hcnse_log_stderr(err, "Failed to init modules");
        goto failed;
    }

    hcnse_pool_destroy(ptemp);

    hcnse_logger_set_global(server->logger);











    while (1);

    return 0;

failed:

#if (HCNSE_WIN32)
    system("pause");
#endif
    return 1;
}
