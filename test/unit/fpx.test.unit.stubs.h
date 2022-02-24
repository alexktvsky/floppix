#ifndef FPX_TEST_UNIT_STUBS_H
#define FPX_TEST_UNIT_STUBS_H

fpx_declare_stub(void *, fpx_malloc1, const char *filename, int line,
    size_t n);
fpx_declare_stub(void *, fpx_palloc1,  const char *filename, int line,
    fpx_pool_t *pool, size_t n);


#endif /* FPX_TEST_UNIT_STUBS_H */
