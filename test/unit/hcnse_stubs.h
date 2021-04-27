#ifndef INCLUDED_HCNSE_STUBS_H
#define INCLUDED_HCNSE_STUBS_H

hcnse_declare_stub(void *, hcnse_malloc, size_t n);
hcnse_declare_stub(void *, hcnse_palloc, hcnse_pool_t *pool, size_t n);


#endif /* INCLUDED_HCNSE_STUBS_H */
