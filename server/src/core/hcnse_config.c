#include "hcnse_portable.h"
#include "hcnse_core.h"



void foo(size_t argc, char **argv) {
    printf("foo ");
    for (size_t i = 0; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");
}



static const hcnse_conf_directive_t hcnse_core_directives[] = {
    {"daemon", HCNSE_CONF_TAKE1, foo},
    {"workdir", HCNSE_CONF_TAKE1, foo},
    {"priority", HCNSE_CONF_TAKE1, foo},
    {"log", HCNSE_CONF_TAKE2|HCNSE_CONF_TAKE3, foo},
    HCNSE_CONF_NULL_DIRECTIVE
};

static char *test =

    "      \n"
    "daemon   on1\n"
    "daemon on2 # comment1\n"
    "daemon on3  # comment2\n"

    "log /tmp/server1.log debug nolimit\n"
    "log syslog debug\n"

    "#daemon on4\n"
    "# daemon on5\n"
    "#  daemon on6\n"
    "#  daemon on7 # comment3\n";


static char *test_buf[512];

static uint32_t hcnse_conf_takes_number[] = {
    HCNSE_CONF_TAKE0,
    HCNSE_CONF_TAKE1,
    HCNSE_CONF_TAKE2,
    HCNSE_CONF_TAKE3,
    HCNSE_CONF_TAKE4,
    HCNSE_CONF_TAKE5,
    HCNSE_CONF_TAKE6,
    HCNSE_CONF_TAKE7
};


static hcnse_err_t
hcnse_conf_parse(hcnse_conf_t *conf)
{
    hcnse_conf_directive_t *directive;
    char *begin;
    char *end;
    size_t len;

    char *ptr;

    size_t argc_min;
    size_t argc_max;
    size_t argc;
    char *argv[7];

    char c;

    bool found;
    bool commit;
    bool in_directive;
    bool end_line;
    bool variadic;


    begin = NULL;
    ptr = test_buf;

    found = false;
    commit = false;
    in_directive = false;
    end_line = false;
    variadic = false;


    for (size_t i = 0; ; i++) {
        c = test[i];

        end_line = false;

        switch (c) {
        case HCNSE_NULL:
            if (in_directive && !variadic) {
                hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
                    "Unexpected end of file, directive \"%s\" is not ended",
                    directive->name);
                return HCNSE_ERR_CONF_SYNTAX;
            }

            if (variadic) {
                if (argc >= argc_min) {
                    directive->handler(argc, argv);
                    in_directive = false;
                    variadic = false;
                }
                else {
                    hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
                        "Unexpected end of file, "
                        "variadic directive \"%s\" is not ended",
                        directive->name);
                    return HCNSE_ERR_CONF_SYNTAX;
                }
            }

            return HCNSE_OK;

        case HCNSE_TAB:
        case HCNSE_SPACE:
            if (begin != NULL) {
                end = &test[i];
                found = true;
            }
            break;

        case HCNSE_LF:
        case HCNSE_CR:
            end_line = true;
            commit = false;
            if (begin != NULL) {
                end = &test[i];
                found = true;
            }
            break;

        case '#':
            if (begin != NULL) {
                hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
                    "Unexpected #, expecting space character "
                    "after parameter or directive");
                return HCNSE_ERR_CONF_SYNTAX;
            }
            commit = true;
            break;

        default:
            if (begin == NULL && !commit) {
                begin = &test[i];
            }
        }

        if (found) {
            len = (size_t) (end - begin);
            hcnse_memmove(ptr, begin, len);
            ptr[len] = HCNSE_NULL;

            for (size_t j = 0; hcnse_core_directives[j].name; j++) {
                if (hcnse_strcmp(hcnse_core_directives[j].name, ptr) == 0) {
                    if (in_directive) {
                        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
                            "Unexpected directive \"%s\", "
                            "directive \"%s\" is not ended",
                            ptr, directive->name);
                        return HCNSE_ERR_CONF_SYNTAX;
                    }
                    directive = &hcnse_core_directives[j];
                    in_directive = true;
                    argc = 0;
                    argc_min = 0;
                    argc_max = 0;

                    
                    for (size_t tn_i = 0; tn_i < 8; tn_i++) {
                        if (directive->takes & hcnse_conf_takes_number[tn_i]) {
                            if (argc_min == 0) {
                                argc_min = tn_i;
                            }
                            else {
                                variadic = true;
                                argc_max = tn_i;
                            }
                        }
                    }

                    goto next;
                }
            }

            if (!in_directive) {
                hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
                    "Unknown directive \"%s\"", ptr);
                return HCNSE_ERR_CONF_SYNTAX;
            }

            if (in_directive) {
                argv[argc] = ptr;
                argc += 1;
                if ((argc == argc_min && !variadic)
                    || (argc == argc_max && variadic))
                {
                    directive->handler(argc, argv);
                    in_directive = false;
                    variadic = false;
                }
            }
            if (in_directive && end_line && !variadic) {
                hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
                    "Unexpected end of line, directive \"%s\" is not ended",
                    directive->name);
                return HCNSE_ERR_CONF_SYNTAX;
            }

            if (in_directive && end_line && variadic) {
                if (argc >= argc_min) {
                    directive->handler(argc, argv);
                    in_directive = false;
                    variadic = false;
                }
                else {
                    hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
                        "Unexpected end of line, "
                        "variadic directive \"%s\" is not ended",
                        directive->name);
                    return HCNSE_ERR_CONF_SYNTAX;
                }
            }
next:
            begin = NULL;
            found = false;
            ptr += len + 1;
        }
        continue;
    }


    return HCNSE_OK;
}

static void
hcnse_conf_set_default_params(hcnse_conf_t *conf)
{
// #if (HCNSE_POSIX)
//     conf->workdir = "/";
//     // conf->log_fname = "server.log";

// #elif (HCNSE_WIN32)
//     conf->workdir = "C:\\";
//     // conf->log_fname = "server.log";
// #endif

//     conf->log_size = 0;
//     conf->log_level = HCNSE_LOG_ERROR;
//     conf->log_rewrite = false;

//     conf->daemon = true;
//     conf->priority = 0;
//     conf->timer = 30000;

//     conf->worker_processes = 0;
//     conf->worker_connections = 0;
}

hcnse_err_t
hcnse_conf_init_and_parse(hcnse_conf_t **in_conf, hcnse_pool_t *pool,
    const char *fname)
{
    hcnse_conf_t *conf;
    hcnse_conf_bin_t *conf_bin;
    hcnse_file_t *file;
    hcnse_err_t err;

    conf = hcnse_pcalloc(pool, sizeof(hcnse_conf_t));
    if (!conf) {
        err = hcnse_get_errno();
        goto failed;
    }

    file = hcnse_palloc(pool, sizeof(hcnse_file_t));
    if (!file) {
        err = hcnse_get_errno();
        goto failed;
    }

    err = hcnse_file_init(file, fname, HCNSE_FILE_RDONLY,
                    HCNSE_FILE_OPEN, HCNSE_FILE_DEFAULT_ACCESS);
    if (err != HCNSE_OK) {
        goto failed;
    }

    hcnse_pool_cleanup_add(pool, file, hcnse_file_fini);




    conf->pool = pool;
    conf->file = file;


    hcnse_conf_set_default_params(conf);


    err = hcnse_conf_parse(conf);
    if (err != HCNSE_OK) {
        goto failed;
    }

    *in_conf = conf;
    return HCNSE_OK;

failed:
    return err;
}

void
hcnse_conf_fini(hcnse_conf_t *conf)
{
    (void) conf;
}


// hcnse_err_t
// hcnse_conf_get_flag_param()
// {

// }

// hcnse_err_t
// hcnse_conf_get_index_param()
// {

// }

// hcnse_err_t
// hcnse_conf_get_size_param()
// {

// }



// const char *
// hcnse_get_metric_prefix(size_t number)
// {
//     size_t i;

//     if (number < HCNSE_METRIC_MULTIPLIER_KILO) {
//         return HCNSE_METRIC_PREFIX_EMPTY;
//     }
//     for (i = 0; number > 0; i++) {
//         if (number % 10 != 0) {
//             break;
//         }
//         number /= 10;
//     }

//     if (i >= 12) {
//         return HCNSE_METRIC_PREFIX_TERA;
//     }
//     else if (i >= 9) {
//         return HCNSE_METRIC_PREFIX_GIGA;
//     }
//     else if (i >= 6) {
//         return HCNSE_METRIC_PREFIX_MEGA;
//     }
//     else if (i >= 3) {
//         return HCNSE_METRIC_PREFIX_KILO;
//     }
//     else {
//         return HCNSE_METRIC_PREFIX_EMPTY;
//     }
// }

// size_t
// hcnse_convert_to_prefix(size_t number)
// {
//     size_t i;
//     size_t temp;

//     temp = number;

//     if (number < HCNSE_METRIC_MULTIPLIER_KILO) {
//         return number;
//     }
//     for (i = 0; number > 0; i++) {
//         if (number % 10 != 0) {
//             break;
//         }
//         number /= 10;
//     }

//     if (i >= 12) {
//         return temp / HCNSE_METRIC_MULTIPLIER_TERA;
//     }
//     else if (i >= 9) {
//         return temp / HCNSE_METRIC_MULTIPLIER_GIGA;
//     }
//     else if (i >= 6) {
//         return temp / HCNSE_METRIC_MULTIPLIER_MEGA;
//     }
//     else if (i >= 3) {
//         return temp / HCNSE_METRIC_MULTIPLIER_KILO;
//     }
//     else {
//         return temp;
//     }
// }
