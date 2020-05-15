#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_MIN_ORDER 1
#define HCNSE_MIN_ALLOC (HCNSE_ALIGN_SIZE << HCNSE_MIN_ORDER)

#define HCNSE_SIZEOF_MEMNODE_T_ALIGN \
    hcnse_align_default(sizeof(hcnse_memnode_t))
#define HCNSE_SIZEOF_MEMPOOL_T_ALIGN \
    hcnse_align_default(sizeof(hcnse_pool_t))

#define HCNSE_MAX_MEMNODE_SIZE(node) \
    ((size_t) (((uintptr_t) node->endp) - ((uintptr_t) node->startp)))

#define HCNSE_PAGE_SIZE (hcnse_get_page_size())

/* 
 * slot  0: size  4096
 * slot  1: size  8192
 * slot  2: size 12288
 * ...
 * slot 19: size 81920
 * slot 20: nodes larger than 81920
 */
#define HCNSE_MAX_POOL_INDEX 20


typedef struct hcnse_memnode_s hcnse_memnode_t;
typedef struct hcnse_cleanup_node_s hcnse_cleanup_node_t;

struct hcnse_memnode_s {
    hcnse_memnode_t *next;
    uint8_t *startp;
    uint8_t *first_avail;
    uint8_t *endp;
    size_t free_size;
};

struct hcnse_cleanup_node_s {
    hcnse_cleanup_node_t *next;
    void *data;
    hcnse_cleanup_handler_t handler;
};

struct hcnse_pool_s {
    hcnse_memnode_t *nodes[HCNSE_MAX_POOL_INDEX];
    hcnse_pool_t *parent;
    hcnse_pool_t *brother;
    hcnse_pool_t *child;

    hcnse_cleanup_node_t *cleanups;
    hcnse_cleanup_node_t *free_cleanups;
    // hcnse_mutex_t mutex;
};


static size_t
hcnse_align_allocation(size_t in_size)
{
    size_t size = hcnse_align_default(in_size);
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

void
hcnse_pool_add_child(hcnse_pool_t *parent, hcnse_pool_t *new_child)
{
    hcnse_pool_t *temp;

    new_child->parent = parent;

    /* New child is not first */
    if (parent->child) {
        temp = parent->child;
        parent->child = new_child;
        new_child->brother = temp;
    }
    /* New child is first */
    else {
        parent->child = new_child;
        new_child->brother = NULL;
    }
}

hcnse_err_t
hcnse_pool_create1(hcnse_pool_t **newpool, hcnse_pool_t *parent)
{
    hcnse_pool_t *pool;
    hcnse_err_t err;

    pool = hcnse_malloc(sizeof(hcnse_pool_t));
    if (!pool) {
        err = hcnse_get_errno();
        goto failed;
    }
    hcnse_memset(pool, 0, sizeof(hcnse_pool_t));

    if (parent) {
        hcnse_pool_add_child(parent, pool);
    }

    *newpool = pool;
    return HCNSE_OK;

failed:
    if (pool) {
        hcnse_free(pool);
    }
    return err;
}

hcnse_pool_t *
hcnse_pool_create(hcnse_pool_t *parent)
{
    hcnse_pool_t *pool;
    if (hcnse_pool_create1(&pool, parent) != HCNSE_OK) {
        return NULL;
    }
    return pool;
}

void *
hcnse_palloc(hcnse_pool_t *pool, size_t in_size)
{
    size_t size;
    size_t npages;
    size_t index;
    void *mem;
    hcnse_memnode_t *node;
    hcnse_memnode_t *temp;

    size = hcnse_align_allocation(in_size);
    if (!size) {
        return NULL;
    }

    /*
     * First see if there are any nodes in the area we know
     * our node will fit into
     */
    npages = hcnse_get_npages(size);
    index = npages - 1;
    if (index > HCNSE_MAX_POOL_INDEX) {
        return NULL;
    }

    node = (pool->nodes)[index];
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
        temp = (pool->nodes)[index];
        (pool->nodes)[index] = node;
        node->next = temp;

        node->free_size -= size;
        mem = node->first_avail;
        node->first_avail += size;
    }
    return mem;
}

void *
hcnse_pcalloc(hcnse_pool_t *pool, size_t in_size)
{
    void *mem;

    mem = hcnse_palloc(pool, in_size);
    if (mem) {
        hcnse_memset(mem, 0, in_size);
    }
    return mem;
}

void
hcnse_pool_cleanup_add1(hcnse_pool_t *pool, void *data,
    hcnse_cleanup_handler_t handler)
{
    hcnse_cleanup_node_t *temp1;

    if (pool->free_cleanups) {
        temp1 = pool->free_cleanups;
        pool->free_cleanups = temp1->next;
    }
    else {
        temp1 = hcnse_palloc(pool, sizeof(hcnse_cleanup_node_t));
    }
    temp1->data = data;
    temp1->handler = handler;
    temp1->next = pool->cleanups;
    pool->cleanups = temp1;
}

void
hcnse_pool_cleanup_run(hcnse_pool_t *pool)
{
    hcnse_cleanup_node_t *temp1;
    hcnse_cleanup_node_t *temp2;

    temp1 = pool->cleanups;

    while (temp1) {
        temp2 = temp1;
        temp1 = temp1->next;

        temp2->handler(temp2->data);
        temp2->next = pool->free_cleanups;
        pool->free_cleanups = temp2;
    }

    pool->cleanups = NULL;
}

size_t
hcnse_pool_get_size(hcnse_pool_t *pool)
{
    size_t size = 0;
    hcnse_memnode_t *node;
    for (size_t i = 0; i < HCNSE_MAX_POOL_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += HCNSE_MAX_MEMNODE_SIZE(node);
            node = node->next;
        }
    }
    return size;
}

size_t
hcnse_pool_get_free_size(hcnse_pool_t *pool)
{
    size_t size = 0;
    hcnse_memnode_t *node;
    for (size_t i = 0; i < HCNSE_MAX_POOL_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += node->free_size;
            node = node->next;
        }
    }
    return size;
}

size_t
hcnse_pool_get_total_size(hcnse_pool_t *pool)
{
    size_t size = 0;
    hcnse_memnode_t *node;
    for (size_t i = 0; i < HCNSE_MAX_POOL_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            size += HCNSE_PAGE_SIZE * (i + 1);
            node = node->next;
        }
    }
    return size;
}

void
hcnse_pool_clean(hcnse_pool_t *pool)
{
    hcnse_memnode_t *node;

    hcnse_pool_cleanup_run(pool);

    if (pool->child) {
        hcnse_pool_t *temp1 = pool->child;
        hcnse_pool_t *temp2;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            hcnse_pool_clean(temp2);
        }
    }

    for (size_t i = 0; i < HCNSE_MAX_POOL_INDEX; i++) {
        node = (pool->nodes)[i];
        while (node) {
            node->first_avail = node->startp;
            node->free_size = HCNSE_MAX_MEMNODE_SIZE(node);
            node = node->next;
        }
    }
}

void
hcnse_pool_destroy(hcnse_pool_t *pool)
{
    hcnse_memnode_t *node;
    hcnse_memnode_t *temp;

    hcnse_pool_cleanup_run(pool);

    if (pool->child) {
        hcnse_pool_t *temp1 = pool->child;
        hcnse_pool_t *temp2;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            hcnse_pool_destroy(temp2);
        }
    }

    for (size_t i = 0; i < HCNSE_MAX_POOL_INDEX; i++) {
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
}
