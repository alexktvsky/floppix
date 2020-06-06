#ifndef INCLUDED_HCNSE_REGEX_H
#define INCLUDED_HCNSE_REGEX_H


struct hcnse_regex_compile_s {
    const char *pattern;
    pcre *re;
    const char *error;
    int erroffset;
};


void hcnse_regex_init(void);
void hcnse_regex_malloc_init(hcnse_pool_t *pool);
void hcnse_regex_malloc_done(void);
hcnse_err_t hcnse_regex_compile(hcnse_regex_compile_t *rc, const char *pattern);
hcnse_err_t hcnse_regex_exec(hcnse_regex_compile_t *rc, const char *str,
    size_t len, int offset, int *captures, size_t captures_size);
void hcnse_regex_destroy(hcnse_regex_compile_t *rc);

#endif /* INCLUDED_HCNSE_REGEX_H */
