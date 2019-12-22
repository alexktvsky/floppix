#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <pthread.h>

#include "platform.h"

#if SYSTEM_WIN32 || SYSTEM_WIN64
#include <windows.h>
#endif

#ifdef ALLOCATE_WITH_MMAP
#if SYSTEM_WIN32 || SYSTEM_WIN64
#error "Allocate with mmap not supported on Windows!"
#elif SYSTEM_LINUX || SYSTEM_FREEBSD || SYSTEM_SOLARIS
#include <sys/mman.h>
#endif
#endif

#include "errors.h"
#include "pools.h"


/* Alignment macros is only to be used to align on a power of 2 boundary */
#define ALIGN_SIZE sizeof(uintptr_t)
#define ALIGN(p, b) \
    (((p) + ((b) - 1)) & ~((b) - 1))
#define ALIGN_DEFAULT(p) ALIGN(p, ALIGN_SIZE)

#define MIN_ORDER 1
#define MIN_ALLOC (ALIGN_SIZE << MIN_ORDER)

#define SIZEOF_MEMNODE_T (sizeof(memnode_t))
#define SIZEOF_POOL_T (sizeof(pool_t))

#define SIZEOF_MEMNODE_T_ALIGN ALIGN_DEFAULT(sizeof(memnode_t))
#define SIZEOF_POOL_T_ALIGN ALIGN_DEFAULT(sizeof(pool_t))

#define MAX_MEMNODE_SIZE(_node) \
    (uint32_t) (uintptr_t) _node->endp - (uintptr_t) _node->startp;

/* slot  0: size  4096
 * slot  1: size  8192
 * slot  2: size 12288
 * ...
 * slot 19: size 81920
 * slot 20: nodes larger than 81920 */
#define MAX_INDEX 20

#define PAGE_SIZE (get_page_size())


typedef struct memnode_s memnode_t;

struct memnode_s {
    memnode_t *next;
    uint8_t *startp;
    uint8_t *first_avail;
    uint8_t *endp;
    uint32_t free_size;
};

struct pool_s {
    memnode_t *nodes[MAX_INDEX];
    pool_t *parent;
    pool_t *brother;
    pool_t *child;

#ifdef _PTHREAD_H
    pthread_mutex_t mutex;
#endif
};


static inline size_t align_allocation(size_t in_size)
{
    size_t size = ALIGN_DEFAULT(in_size);
    if (size < in_size) {
        return 0;
    }
    if (size < MIN_ALLOC) {
        size = MIN_ALLOC;
    }
    return size;
}


static inline size_t get_page_size(void)
{
    size_t page_size;
#if defined (_SC_PAGESIZE)
    page_size = sysconf(_SC_PAGESIZE);
#elif SYSTEM_WIN32 || SYSTEM_WIN64
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    page_size = si.dwPageSize;
#else
#error "Error of determine page size!"
#endif
    return page_size;
}


static inline size_t get_npages(size_t in_size)
{
    size_t size = in_size + SIZEOF_MEMNODE_T_ALIGN;
    if (size < PAGE_SIZE) {
        return 1;
    }
    else {
        return ((size / PAGE_SIZE) + 1);
    }
}


static memnode_t *memnode_allocate_and_init(size_t in_size)
{
    memnode_t *node;

#ifdef ALLOCATE_WITH_MMAP
    node = mmap(NULL, in_size, PROT_READ|PROT_WRITE,
                MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#else
    node = malloc(in_size);
#endif

    if (!node) {
        return NULL;
    }

    /* Initialization of memnode */
    node->next = NULL;
    node->startp = (uint8_t *) node + SIZEOF_MEMNODE_T_ALIGN;
    node->first_avail = (uint8_t *) node->startp;
    node->endp = (uint8_t *) node + in_size;
    node->free_size = MAX_MEMNODE_SIZE(node);

    return node;
}


status_t pool_create(pool_t **newpool, pool_t *parent)
{
    pool_t *pool = malloc(SIZEOF_POOL_T);
    /* XXX: Can we do smth if malloc return NULL?
     * Allocate from stack or text section instead?
     */
    if (!pool) {
        return ALLOC_MEM_ERROR;
    }
    memset(pool, 0, SIZEOF_POOL_T);

    if (parent) {
        pool->parent = parent;
        /* If new pool is first child */
        if (!(parent->child)) {
            parent->child = pool;
        }
        /* If new pool is not first child */
        else {
            /* Go to end of list */
            pool_t *temp1 = parent->child;
            pool_t *temp2;
            while (temp1) {
                temp2 = temp1;
                temp1 = temp1->brother;
            }
            temp2->brother = pool;
        }
    }

    *newpool = pool;
    return XXX_OK;
}


void *palloc(pool_t *pool, size_t in_size)
{
    size_t size = align_allocation(in_size);
    if (!size) {
        return NULL;
    }

    /* First see if there are any nodes in the area we know
     * our node will fit into. */
    size_t npages = get_npages(size);
    size_t index = npages - 1;
    if (index > MAX_INDEX) {
        return NULL;
    }
    void *mem;
    memnode_t *node = (pool->nodes)[index];
    while (node) {
        if (node->free_size >= size) {
            node->free_size -= size;
            mem = node->first_avail;
            node->first_avail += size;
            break;
        }
        node = node->next;
    }

    /* If we haven't got a suitable node, allocate a new one */
    if (!node) {
        node = memnode_allocate_and_init(npages * PAGE_SIZE);

        /* Registaration of memnode */
        memnode_t *temp1 = (pool->nodes)[index];
        memnode_t *temp2;
        /* If array is empty */
        if (!temp1) {
            (pool->nodes)[index] = node;
        }
        /* If array is not empty */
        else {
            /* Go to the end of list */
            while (temp1) {
                temp2 = temp1;
                temp1 = temp1->next;
            }
            temp2->next = node;
        }

        node->free_size -= size;
        mem = node->first_avail;
        node->first_avail += size;
    }
    return mem;
}


void *pcalloc(pool_t *pool, size_t in_size)
{
    void *mem;
    if ((mem = palloc(pool, in_size)) != NULL) {
        memset(mem, 0, in_size);
    }
    return mem;
}


size_t get_pool_size(pool_t *pool)
{
    size_t size = 0;
    memnode_t *node;
    for (size_t i = 0; i < MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += MAX_MEMNODE_SIZE(node);
            node = node->next;
        }
    }
    return size;
}


size_t get_pool_free_size(pool_t *pool)
{
    size_t size = 0;
    memnode_t *node;
    for (size_t i = 0; i < MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += node->free_size;
            node = node->next;
        }
    }
    return size;
}


size_t get_pool_total_size(pool_t *pool)
{
    size_t size = 0;
    memnode_t *node;
    for (size_t i = 0; i < MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += PAGE_SIZE * (i + 1);
            node = node->next;
        }
    }
    return size;
}


void pool_clear(pool_t *pool)
{
    if (pool->child) {
        pool_t *temp1 = pool->child;
        pool_t *temp2;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            pool_clear(temp2);
        }
    }

    memnode_t *node; 
    for (size_t i = 0; i < MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            node->first_avail = node->startp;
            node->free_size = MAX_MEMNODE_SIZE(node);
            node = node->next;
        }
    }
}


void pool_destroy(pool_t *pool)
{
    if (pool->child) {
        pool_t *temp1 = pool->child;
        pool_t *temp2;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            pool_destroy(temp2);
        }
    }

    memnode_t *node;
    memnode_t *temp;    
    for (size_t i = 0; i < MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            temp = node;
            node = node->next;
#ifdef ALLOCATE_WITH_MMAP
            munmap(temp, PAGE_SIZE * (i + 1));
#else
            free(temp);
#endif
        }
    }
    free(pool);
}
