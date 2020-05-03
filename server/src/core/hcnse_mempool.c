#include "hcnse_portable.h"
#include "hcnse_core.h"


/* Alignment macros is only to be used to align on a power of 2 boundary */
#define HCNSE_ALIGN_SIZE sizeof(uintptr_t)
#define HCNSE_ALIGN(p, b) \
    (((p) + ((b) - 1)) & ~((b) - 1))
#define HCNSE_ALIGN_DEFAULT(p) HCNSE_ALIGN(p, HCNSE_ALIGN_SIZE)

#define HCNSE_MIN_ORDER 1
#define HCNSE_MIN_ALLOC (HCNSE_ALIGN_SIZE << HCNSE_MIN_ORDER)

#define HCNSE_SIZEOF_MEMNODE_T (sizeof(hcnse_memnode_t))
#define HCNSE_SIZEOF_MEMPOOL_T (sizeof(hcnse_mempool_t))

#define HCNSE_SIZEOF_MEMNODE_T_ALIGN \
    HCNSE_ALIGN_DEFAULT(sizeof(hcnse_memnode_t))
#define HCNSE_SIZEOF_MEMPOOL_T_ALIGN \
    HCNSE_ALIGN_DEFAULT(sizeof(hcnse_mempool_t))

#define HCNSE_MAX_MEMNODE_SIZE(node) \
    ((size_t) (uintptr_t) node->endp - (uintptr_t) node->startp);

/* slot  0: size  4096
 * slot  1: size  8192
 * slot  2: size 12288
 * ...
 * slot 19: size 81920
 * slot 20: nodes larger than 81920 */
#define HCNSE_MAX_INDEX 20

#define HCNSE_PAGE_SIZE (hcnse_get_page_size())


typedef struct hcnse_memnode_s hcnse_memnode_t;

struct hcnse_memnode_s {
    hcnse_memnode_t *next;
    uint8_t *startp;
    uint8_t *first_avail;
    uint8_t *endp;
    size_t free_size;
};

struct hcnse_mempool_s {
    hcnse_memnode_t *nodes[HCNSE_MAX_INDEX];
    hcnse_mempool_t *parent;
    hcnse_mempool_t *brother;
    hcnse_mempool_t *child;

    // hcnse_mutex_t mutex;
};


static size_t
hcnse_align_allocation(size_t in_size)
{
    size_t size = HCNSE_ALIGN_DEFAULT(in_size);
    if (size < in_size) {
        return 0;
    }
    if (size < HCNSE_MIN_ALLOC) {
        size = HCNSE_MIN_ALLOC;
    }
    return size;
}

static size_t
hcnse_get_npages(size_t in_size)
{
    size_t size = in_size + HCNSE_SIZEOF_MEMNODE_T_ALIGN;
    if (size < HCNSE_PAGE_SIZE) {
        return 1;
    }
    else {
        return ((size / HCNSE_PAGE_SIZE) + 1);
    }
}

static hcnse_memnode_t *
hcnse_memnode_allocate_and_init(size_t in_size)
{
    hcnse_memnode_t *node;

#if (HCNSE_HAVE_MMAP)
    node = mmap(NULL, in_size, PROT_READ|PROT_WRITE,
                                MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (node == MAP_FAILED) {
        return NULL;
    }
#else
    node = hcnse_malloc(in_size);
    if (node == NULL) {
        return NULL;
    }
#endif

    /* Initialization of memnode */
    node->next = NULL;
    node->startp = (uint8_t *) node + HCNSE_SIZEOF_MEMNODE_T_ALIGN;
    node->first_avail = (uint8_t *) node->startp;
    node->endp = (uint8_t *) node + in_size;
    node->free_size = HCNSE_MAX_MEMNODE_SIZE(node);
    return node;
}

hcnse_err_t
hcnse_mempool_create(hcnse_mempool_t **newpool, hcnse_mempool_t *parent)
{
    hcnse_mempool_t *pool = hcnse_malloc(HCNSE_SIZEOF_MEMPOOL_T);
    /* XXX: Can we do smth if malloc return NULL?
     * Allocate from stack or text section instead?
     */
    if (!pool) {
        return hcnse_get_errno();
    }
    memset(pool, 0, HCNSE_SIZEOF_MEMPOOL_T);

    if (parent) {
        pool->parent = parent;
        /* If new pool is first child */
        if (!(parent->child)) {
            parent->child = pool;
        }
        /* If new pool is not first child */
        else {
            /* Go to end of list */
            hcnse_mempool_t *temp1 = parent->child;
            hcnse_mempool_t *temp2;
            while (temp1) {
                temp2 = temp1;
                temp1 = temp1->brother;
            }
            temp2->brother = pool;
        }
    }

    *newpool = pool;
    return HCNSE_OK;
}

void *
hcnse_palloc(hcnse_mempool_t *pool, size_t in_size)
{
    size_t size = hcnse_align_allocation(in_size);
    if (!size) {
        return NULL;
    }

    /* First see if there are any nodes in the area we know
     * our node will fit into. */
    size_t npages = hcnse_get_npages(size);
    size_t index = npages - 1;
    if (index > HCNSE_MAX_INDEX) {
        return NULL;
    }
    void *mem;
    hcnse_memnode_t *node = (pool->nodes)[index];
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
        node = hcnse_memnode_allocate_and_init(npages * HCNSE_PAGE_SIZE);

        /* Registaration of memnode */
        hcnse_memnode_t *temp1 = (pool->nodes)[index];
        hcnse_memnode_t *temp2;
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

void *
hcnse_pcalloc(hcnse_mempool_t *pool, size_t in_size)
{
    void *mem;
    if ((mem = hcnse_palloc(pool, in_size)) != NULL) {
        memset(mem, 0, in_size);
    }
    return mem;
}

size_t
hcnse_get_mempool_size(hcnse_mempool_t *pool)
{
    size_t size = 0;
    hcnse_memnode_t *node;
    for (size_t i = 0; i < HCNSE_MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += HCNSE_MAX_MEMNODE_SIZE(node);
            node = node->next;
        }
    }
    return size;
}

size_t
hcnse_get_mempool_free_size(hcnse_mempool_t *pool)
{
    size_t size = 0;
    hcnse_memnode_t *node;
    for (size_t i = 0; i < HCNSE_MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += node->free_size;
            node = node->next;
        }
    }
    return size;
}

size_t
hcnse_get_mempool_total_size(hcnse_mempool_t *pool)
{
    size_t size = 0;
    hcnse_memnode_t *node;
    for (size_t i = 0; i < HCNSE_MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += HCNSE_PAGE_SIZE * (i + 1);
            node = node->next;
        }
    }
    return size;
}

void
hcnse_mempool_clear(hcnse_mempool_t *pool)
{
    if (pool->child) {
        hcnse_mempool_t *temp1 = pool->child;
        hcnse_mempool_t *temp2;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            hcnse_mempool_clear(temp2);
        }
    }

    hcnse_memnode_t *node; 
    for (size_t i = 0; i < HCNSE_MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            node->first_avail = node->startp;
            node->free_size = HCNSE_MAX_MEMNODE_SIZE(node);
            node = node->next;
        }
    }
    return;
}

void
hcnse_mempool_destroy(hcnse_mempool_t *pool)
{
    if (pool->child) {
        hcnse_mempool_t *temp1 = pool->child;
        hcnse_mempool_t *temp2;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            hcnse_mempool_destroy(temp2);
        }
    }

    hcnse_memnode_t *node;
    hcnse_memnode_t *temp;    
    for (size_t i = 0; i < HCNSE_MAX_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            temp = node;
            node = node->next;
#if (HCNSE_HAVE_MMAP)
            munmap(temp, HCNSE_PAGE_SIZE * (i + 1));
#else
            hcnse_free(temp);
#endif
        }
    }
    hcnse_free(pool);
    return;
}
