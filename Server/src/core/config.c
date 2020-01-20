#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "platform.h"
#include "errors.h"
#include "sockets.h"
#include "listen.h"
#include "mempool.h"
#include "config.h"

#if (SYSTEM_WINDOWS)
#define PCRE_STATIC
#endif
#include <pcre.h>


#define MAX_SIZE_LINE     100
#define SIZE    (sizeof(patterns)/MAX_SIZE_LINE)

#define MAX_SECTION_DEEP  30
#define SECTION_SERVER    1
#define SECTION_INTERFACE 2


static bool was_set_filename = false;
static bool was_init = false;
static bool was_read = false;

static struct {
    /* Name of config file */
    char *config_filename;
    /* Listeners */
    listen_unit_t *listeners;
    /* Number of logic CPU */
    size_t nprocs;
    /* Path to log file */
    char *logfile;
    /* Limit size of log file */
    size_t maxlog;
    /* Private field */
    mempool_t *pool;
} config;


#define SafeIncSectionCounter() {                                       \
    if (section_counter == MAX_SECTION_DEEP) {                          \
        return CONF_OVERFLOW_ERROR; /* Parser section overflow */       \
    }                                                                   \
    section_counter++;                                                  \
}

#define SafeDecSectionCounter() {                                       \
    if (section_counter > 0) {                                          \
        section_counter--;                                              \
    }                                                                   \
}


status_t init_config(void)
{
    was_init = true;
    mempool_t *newpool;
    if (mempool_create(&newpool, NULL) != XXX_OK) {
        return ALLOC_MEM_ERROR;
    }
    config.pool = newpool;
    return XXX_OK;
}


void fini_config(void)
{
    if (!was_init) {
        return;
    }
    was_init = false;
    was_set_filename = false;
    was_read = false;
    if (config.pool) {
        mempool_destroy(config.pool);
    }
}

status_t set_config_filename(char *in_filename)
{
    if (!was_init) {
        return XXX_FAILED;
    }
    if (!in_filename) {
        return NULL_ADDRESS_ERROR; /* Error of null address */
    }
    was_set_filename = true;
    config.config_filename = in_filename;
    return XXX_OK;
}


listen_unit_t *config_get_listeners(void)
{
    if (!was_read) {
        return NULL;
    }
    return config.listeners;
}


size_t config_get_nprocs(void)
{
    if (!was_read) {
        return 0;
    }
    return config.nprocs;
}


char *config_get_logfile(void)
{
    if (!was_read) {
        return NULL;
    }
    return config.logfile;
}


size_t config_get_maxlog(void)
{
    if (!was_read) {
        return 0;
    }
    return config.maxlog;
}


status_t parse_config(void)
{
    if (!was_init) {
        return XXX_FAILED;
    }

    if (!was_set_filename) {
        return XXX_FAILED;
    }

    was_read = true;
    FILE *conf_file = fopen(config.config_filename, "r");
    if (!conf_file) {
        return CONF_OPEN_ERROR; /* Error while open config file */
    }

    char patterns[][MAX_SIZE_LINE] = {
        /* "}", parent: --- */
        "(?<!#)}",                                                             // 0
        /* "server {", parent: --- */
        "(?<!#server\\s)(server\\s*{)",                                        // 1
        /* "interface <interface> {", parent: server */
        "(?<!#interface\\s)(?<=interface\\s)([0-9/a-z/A-Z][^{/\\s]+)(?=.*{)",  // 2
        /* "listen <port> tcp;", parent: interface */
        "(?<!#listen\\s)(?<=listen\\s)([0-9]*[^;]+)(?=.*tcp;)",                // 3
        /* "listen <port> tcp6;", parent: interface */
        "(?<!#listen\\s)(?<=listen\\s)([0-9]*[^;]+)(?=.*tcp6;)",               // 4
        /* "nprocs <nprocs>;", parent: server */
        "(?<!#nprocs\\s)(?<=nprocs\\s)([0-9]*[^;]+)(?=.*;)",                   // 5
        /* "logfile <logfile>;", parent: server */
        "(?<!#logfile\\s)(?<=logfile\\s)([0-9/a-z/A-Z]*[^;]+)(?=.*;)",         // 6
        /* "maxlog <maxlog>;", parent: server */
        "(?<!#maxlog\\s)(?<=maxlog\\s)([0-9]*[^;]+)(?=.*;)"};                  // 7

    const char *pcre_errors[SIZE];
    int pcre_erroroffset[SIZE];
    pcre *pcre_array[SIZE];
    for (size_t i = 0; i < SIZE; i++) {
        pcre_array[i] = pcre_compile(patterns[i], 0, &pcre_errors[i], 
                                     &pcre_erroroffset[i], NULL);
        if (!pcre_array[i]) {
           return CONF_REGEX_ERROR; /* Error of compiling regular expressions */
        }
    }

    config.listeners = NULL;
    config.nprocs = 0;
    config.maxlog = 0;
    
    char last_interface[MAX_SIZE_LINE];
    char input_str[MAX_SIZE_LINE];
    char *pstr = input_str;

    int sections[MAX_SECTION_DEEP];
    int section_counter = 0;

    size_t string_len;

    listen_unit_t *temp1 = NULL;
    listen_unit_t *temp2 = NULL;

    while (pstr) {
        memset(pstr, 0, MAX_SIZE_LINE);
        pstr = fgets(pstr, MAX_SIZE_LINE, conf_file);
        if (pstr) {
            for (size_t i = 0; i < SIZE; i++) {
                int offset = 0;
                int ovector[100];
                while (1) {
                    int count = pcre_exec(pcre_array[i], NULL, pstr, strlen(pstr),
                                          offset, 0, ovector, 100);
                    if (count < 0) {
                        break;
                    }
                    offset = ovector[1];
                    const char **stringlist;
                    if (pcre_get_substring_list(pstr, ovector, count, &stringlist) < 0) {
                        return CONF_SUBSTR_ERROR; /* Error of pcre substring list */
                    }
                    switch (i) {
                        /* End section */
                        case 0:
                            SafeDecSectionCounter();
                            break;
                        /* Section server */
                        case 1:
                            sections[section_counter] = SECTION_SERVER;
                            SafeIncSectionCounter();
                            break;

                        /* Section interface */
                        case 2:
                            if (sections[section_counter - 1] != SECTION_SERVER) {
                                /* Error of syntax configure file */
                                return CONF_SYNTAX_ERROR;
                            }
                            memset(last_interface, 0, MAX_SIZE_LINE);
                            string_len = strlen(stringlist[0]);
                            memmove(last_interface, stringlist[0], string_len);
                            last_interface[string_len] = '\0';
                            sections[section_counter] = SECTION_INTERFACE;
                            SafeIncSectionCounter();
                            break;

                        /* Item listen TCP IPv4 */
                        case 3:
                            if (sections[section_counter - 1] != SECTION_INTERFACE) {
                                /* Error of syntax configure file */
                                return CONF_SYNTAX_ERROR;
                            }
                            temp1 = config.listeners;
                            if (!temp1) {
                                config.listeners = palloc(config.pool, sizeof(listen_unit_t));
                                temp1 = config.listeners;
                                if (!temp1) {
                                    return ALLOC_MEM_ERROR; /* Error of allocate memory */
                                }
                            }
                            else {
                                while (temp1) {
                                    temp2 = temp1;
                                    temp1 = temp1->next;
                                }
                                temp1 = palloc(config.pool, sizeof(listen_unit_t));
                                if (!temp1) {
                                    return ALLOC_MEM_ERROR; /* Error of allocate memory */
                                }
                                temp2->next = temp1;
                            }

                            string_len = strlen(last_interface);

                            temp1->netface = palloc(config.pool, string_len + 1);
                            if (!(temp1->netface)) {
                                return ALLOC_MEM_ERROR; /* Error of allocate memory */
                            }
                            memmove(temp1->netface, last_interface, string_len);
                            (temp1->netface)[string_len] = '\0';
                            temp1->port = atoi(stringlist[0]);
                            temp1->protocol = LISTEN_TCP;
                            temp1->socket = 0;
                            break;

                        /* Item listen TCP IPv6 */
                        case 4:
                            if (sections[section_counter - 1] != SECTION_INTERFACE) {
                                /* Error of syntax configure file */
                                return CONF_SYNTAX_ERROR;
                            }
                            temp1 = config.listeners;
                            if (!temp1) {
                                config.listeners = palloc(config.pool, sizeof(listen_unit_t));
                                temp1 = config.listeners;
                                if (!temp1) {
                                    return ALLOC_MEM_ERROR; /* Error of allocate memory */
                                }
                            }
                            else {
                                while (temp1) {
                                    temp2 = temp1;
                                    temp1 = temp1->next;
                                }
                                temp1 = palloc(config.pool, sizeof(listen_unit_t));
                                if (!temp1) {
                                    return ALLOC_MEM_ERROR; /* Error of allocate memory */
                                }
                                temp2->next = temp1;
                            }

                            string_len = strlen(last_interface);

                            temp1->netface = palloc(config.pool, string_len + 1);
                            if (!(temp1->netface)) {
                                return ALLOC_MEM_ERROR; /* Error of allocate memory */
                            }
                            memmove(temp1->netface, last_interface, string_len);
                            (temp1->netface)[string_len] = '\0';
                            temp1->port = atoi(stringlist[0]);
                            temp1->protocol = LISTEN_TCP6;
                            temp1->socket = 0;
                            break;

                        /* Item nprocs */
                        case 5:
                            if (sections[section_counter - 1] != SECTION_SERVER) {
                                /* Error of syntax configure file */
                                return CONF_SYNTAX_ERROR;
                            }
                            config.nprocs = atoi(stringlist[0]);
                            /* Logic or syntax error */
                            if (config.nprocs == 0) {
                                config.nprocs = 1;
                            }
                            break;

                        /* Item logfile */
                        case 6:
                            if (sections[section_counter - 1] != SECTION_SERVER) {
                                /* Error of syntax configure file */
                                return CONF_SYNTAX_ERROR;
                            }

                            string_len = strlen(stringlist[0]);

                            config.logfile = palloc(config.pool, string_len + 1);
                            memmove(config.logfile, stringlist[0], string_len);
                            config.logfile[string_len] = '\0';
                            break;

                        /* Item maxlog */
                        case 7:
                            if (sections[section_counter - 1] != SECTION_SERVER) {
                                /* Error of syntax configure file */
                                return CONF_SYNTAX_ERROR;
                            }
                            config.maxlog = atoi(stringlist[0]);
                            break;
                    }
                    pcre_free_substring_list(stringlist);
                }
            }
        }
    }
    if (section_counter) {
        return CONF_SYNTAX_ERROR; /* Error of syntax configure file */
    }
    for (size_t i = 0; i < SIZE; i++) {
        pcre_free(pcre_array[i]);
    }
    fclose(conf_file);
    return XXX_OK;
}
