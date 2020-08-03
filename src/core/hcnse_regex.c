#include "hcnse_portable.h"
#include "hcnse_core.h"


static hcnse_pool_t *hcnse_regex_pool;


static void *
hcnse_regex_malloc(size_t size)
{
    hcnse_pool_t *pool;
    pool = hcnse_regex_pool;

    if (pool) {
        return hcnse_palloc(pool, size);
    }

    return NULL;
}

static void
hcnse_regex_free(void *mem)
{
    (void) mem;
    return;
}

void
hcnse_regex_init(void)
{
    pcre_malloc = hcnse_regex_malloc;
    pcre_free = hcnse_regex_free;
}

void
hcnse_regex_malloc_init(hcnse_pool_t *pool)
{
    hcnse_regex_pool = pool;
}


void
hcnse_regex_malloc_done(void)
{
    hcnse_regex_pool = NULL;
}


hcnse_err_t
hcnse_regex_compile(hcnse_regex_compile_t *rc, const char *pattern)
{
    pcre *re;

    re = pcre_compile(pattern, PCRE_MULTILINE, &(rc->error),
        &(rc->erroffset), 0);
    if (!re) {
        goto failed;
    }
    rc->pattern = pattern;
    rc->re = re;

    return HCNSE_OK;

failed:
    if (re) {
        pcre_free(re);
    }
    return HCNSE_FAILED;
}

hcnse_err_t
hcnse_regex_exec(hcnse_regex_compile_t *rc, const char *str, size_t len,
    int offset, int *captures, size_t captures_size)
{
    int ret;
    ret = pcre_exec(rc->re, 0, str, len, offset, 0, captures,
        captures_size);
    if (ret < 0) {
        return HCNSE_DECLINED;
    }

   return HCNSE_OK;
}

void
hcnse_regex_destroy(hcnse_regex_compile_t *rc)
{
    pcre_free(rc->re);
}
