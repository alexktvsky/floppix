#include "fpx.system.os.portable.h"
#include "fpx.system.errno.h"
#include "fpx.system.memory.h"
#include "fpx.util.string.h"
#include "fpx.core.command.h"
#include "fpx.core.module.h"
#include "fpx.core.cycle.h"
#include "fpx.core.release.h"


#if !(FPX_TEST)

static fpx_bool_t show_version;
static fpx_bool_t show_help;
static fpx_bool_t test_config;
static const char *config_fname = FPX_DEFAULT_CONFIG_PATH;


static fpx_err_t
fpx_parse_argv(fpx_uint_t argc, const char *const *argv)
{
    fpx_uint_t i, saved_index;
    const char *p;
    fpx_bool_t long_option;

    for (i = 1; i < argc; ++i) {

        saved_index = i;
        p = argv[i];

        if (*p++ != '-' || *p == '\0') {
            return FPX_FAILED;
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

            if (fpx_strcmp(p, "version") == 0) {
                show_version = true;
                p += sizeof("version") - 1;
                continue;
            }

            if (fpx_strcmp(p, "help") == 0) {
                show_help = true;
                p += sizeof("help") - 1;
                continue;
            }

            if (fpx_strcmp(p, "test-config") == 0) {
                test_config = true;
                p += sizeof("test-config") - 1;
                continue;
            }

            if (fpx_strcmp(p, "config-file") == 0) {
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

    return FPX_OK;

invalid_option:
    fpx_log_stderr(FPX_OK, "Invalid options \"%s\"", argv[saved_index]);
    return FPX_FAILED;

missing_argment:
    fpx_log_stderr(FPX_OK, "Missing argment for options \"%s\"",
        argv[saved_index]);
    return FPX_FAILED;
}

static void
fpx_show_version_info(void)
{
    fpx_log_stdout(FPX_OK, "FPX %s %s",
        FPX_VERSION_STR, FPX_BUILD_DATE);
    fpx_log_stdout(FPX_OK, "Target system: %s %d-bit",
        FPX_SYSTEM_NAME, FPX_PTR_WIDTH);
#ifdef FPX_COMPILER
    fpx_log_stdout(FPX_OK, "Built by %s", FPX_COMPILER);
#endif
}

static void
fpx_show_help_info(void)
{
    fpx_log_stdout(FPX_OK,
        "Usage: fpx [options...] [argments...]\n"
        "FPX version %s %s\n\n"
        "Options:\n"
        "  -h, --help                     Displays this message.\n"
        "  -v, --version                  Displays version information.\n"
        "  -t, --test                     Test configuration and exit.\n"
        "  -c, --config-file <file>       Specify configuration file.",
        FPX_VERSION_STR, FPX_BUILD_DATE);
}

static void
fpx_save_argv(fpx_server_t *server, fpx_int_t argc, const char *const *argv)
{
    server->argc = argc;
    server->argv = argv;
}

int
main(int argc, const char *const *argv)
{
    fpx_server_t *server;
    fpx_pool_t *pool;
    fpx_pool_t *ptemp; /* Pool for temporary config stuff */
    fpx_config_t *config;

    fpx_list_t *modules;
    fpx_err_t err;


    if ((err = fpx_parse_argv(argc, argv)) != FPX_OK) {
        fpx_show_help_info();
        goto failed;
    }

    if (show_version) {
        fpx_show_version_info();
        return 0;
    }
    if (show_help) {
        fpx_show_help_info();
        return 0;
    }

#if (FPX_WIN32)
    if ((err = fpx_winsock_init_v22()) != FPX_OK) {
        fpx_log_stderr(FPX_OK, "Failed to initialize Winsock 2.2");
        goto failed;
    }
#endif

    fpx_assert(fpx_pool_create(&pool, 0, NULL) == FPX_OK);
    fpx_assert(fpx_pool_create(&ptemp, 0, NULL) == FPX_OK);

    fpx_assert(server = fpx_pcalloc(pool, sizeof(fpx_server_t)));

    fpx_save_argv(server, argc, argv);

    fpx_assert(fpx_list_init(&modules, pool) == FPX_OK);
    fpx_assert(config = fpx_palloc(ptemp, sizeof(fpx_config_t)));

    if ((err = fpx_config_read(config, ptemp, config_fname)) != FPX_OK) {
        fpx_log_stderr(err, "Failed to read config file");
        goto failed;
    }

    server->pool = pool;
    server->config = config;
    server->modules = modules;

    if ((err = fpx_setup_prelinked_modules(server)) != FPX_OK) {
        fpx_log_stderr(err, "Failed to setup prelinked modules");
        goto failed;
    }

    if ((err = fpx_preinit_modules(server)) != FPX_OK) {
        fpx_log_stderr(err, "Failed to preinit modules");
        goto failed;
    }

    if (test_config) {
        if (fpx_config_check(config, server) == FPX_OK) {
            fpx_log_stderr(FPX_OK, "Configuration check succeeded");
        }
        else {
            fpx_log_stderr(FPX_OK, "Configuration check failed");
        }
        fpx_logger_destroy(server->logger);
        fpx_pool_destroy(ptemp);
        fpx_pool_destroy(pool);
        return 0;
    }

    if ((err = fpx_config_process(config, server)) != FPX_OK) {
        fpx_log_stderr(err, "Failed to process config");
        goto failed;
    }

    if ((err = fpx_init_modules(server)) != FPX_OK) {
        fpx_log_stderr(err, "Failed to init modules");
        goto failed;
    }

    /* Fixme: Segfault with flag FPX_POOL_USE_MMAP */
    fpx_pool_destroy(ptemp);

    fpx_logger_set_global(server->logger);

    fpx_server_cycle(server);

    return 0;

failed:

#if (FPX_WIN32)
    system("pause");
#endif
    return 1;
}

#endif
