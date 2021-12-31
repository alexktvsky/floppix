#include "hcnse.system.os.portable.h"
#include "hcnse.system.errno.h"
#include "hcnse.system.memory.h"
#include "hcnse.util.string.h"
#include "hcnse.core.command.h"
#include "hcnse.core.module.h"
#include "hcnse.core.cycle.h"
#include "hcnse.release.h"


#if !(HCNSE_TEST)

static bool show_version;
static bool show_help;
static bool test_config;
static const char *config_fname = HCNSE_DEFAULT_CONFIG_PATH;


static hcnse_err_t
hcnse_parse_argv(hcnse_uint_t argc, const char *const *argv)
{
    hcnse_uint_t i, saved_index;
    const char *p;
    bool long_option;

    for (i = 1; i < argc; ++i) {

        saved_index = i;
        p = argv[i];

        if (*p++ != '-' || *p == '\0') {
            return HCNSE_FAILED;
        }

        while (*p) {

            long_option = false;

            switch (*p++) {
            case 'v':
                show_version = true;
                break;

            case 'h':
                show_help = true;
                break;

            case 't':
                test_config = true;
                break;

            case 'c':
                if (argv[++i]) {
                    config_fname = argv[i];
                }
                else {
                    goto missing_argment;
                }
                break;

            case '-':
                long_option = true;
                break;

            default:
                goto invalid_option;
            }

            if (!long_option) {
                continue;
            }

            if (hcnse_strcmp(p, "version") == 0) {
                show_version = true;
                p += sizeof("version") - 1;
                continue;
            }

            if (hcnse_strcmp(p, "help") == 0) {
                show_help = true;
                p += sizeof("help") - 1;
                continue;
            }

            if (hcnse_strcmp(p, "test-config") == 0) {
                test_config = true;
                p += sizeof("test-config") - 1;
                continue;
            }

            if (hcnse_strcmp(p, "config-file") == 0) {
                if (argv[++i]) {
                    config_fname = argv[i];
                    p += sizeof("config-file") - 1;
                    continue;
                }
                else {
                    goto missing_argment;
                }
            }

            goto invalid_option;
        }
    }

    return HCNSE_OK;

invalid_option:
    hcnse_log_stderr(HCNSE_OK, "Invalid options \"%s\"", argv[saved_index]);
    return HCNSE_FAILED;

missing_argment:
    hcnse_log_stderr(HCNSE_OK, "Missing argment for options \"%s\"",
        argv[saved_index]);
    return HCNSE_FAILED;
}

static void
hcnse_show_version_info(void)
{
    hcnse_log_stdout(HCNSE_OK, "HCNSE %s %s",
        HCNSE_VERSION_STR, HCNSE_BUILD_DATE);
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
        hcnse_log_stderr(HCNSE_OK, "Failed to initialize Winsock 2.2");
        goto failed;
    }
#endif

    hcnse_assert(hcnse_pool_create(&pool, 0, NULL) == HCNSE_OK);
    hcnse_assert(hcnse_pool_create(&ptemp, 0, NULL) == HCNSE_OK);

    hcnse_assert(server = hcnse_pcalloc(pool, sizeof(hcnse_server_t)));

    hcnse_save_argv(server, argc, argv);

    hcnse_assert(hcnse_list_init(&modules, pool) == HCNSE_OK);
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
        hcnse_logger_destroy(server->logger);
        hcnse_pool_destroy(ptemp);
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

    /* Fixme: Segfault with flag HCNSE_POOL_USE_MMAP */
    hcnse_pool_destroy(ptemp);

    hcnse_logger_set_global(server->logger);

    hcnse_server_cycle(server);

    return 0;

failed:

#if (HCNSE_WIN32)
    system("pause");
#endif
    return 1;
}

#endif
