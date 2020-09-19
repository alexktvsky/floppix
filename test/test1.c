#include "hcnse_unit_test.h"
#include "hcnse_portable.h"
#include "hcnse_core.h"

hcnse_declare_mock_head(void *, hcnse_malloc, size_t n);
hcnse_declare_mock_body(hcnse_malloc, n);

hcnse_declare_mock_head(void *, hcnse_palloc, hcnse_pool_t *pool, size_t n);
hcnse_declare_mock_body(hcnse_palloc, pool, n);


void test_a1(void) {

    hcnse_will_return(hcnse_malloc, NULL);
    printf("%p\n", hcnse_malloc(10));
    printf("%p\n", hcnse_malloc(10));

    hcnse_will_return(hcnse_palloc, NULL);
    printf("%p\n", hcnse_palloc(NULL, 0));
}


int main(void) {

    test_a1();

    return 0;
}
