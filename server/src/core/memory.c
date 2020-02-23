#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "memory.h"


void explicit_memzero(void *buf, size_t n) {
    memset(buf, 0, n);
    memory_barrier();
    return;
}
