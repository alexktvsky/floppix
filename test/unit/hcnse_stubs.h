#ifndef INCLUDED_FPX_STUBS_H
#define INCLUDED_FPX_STUBS_H

fpx_declare_stub(void *, fpx_malloc, size_t n);
fpx_declare_stub(void *, fpx_palloc, fpx_pool_t *pool, size_t n);


#endif /* INCLUDED_FPX_STUBS_H */
