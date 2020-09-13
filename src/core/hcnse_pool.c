#include "hcnse_portable.h"
#include "hcnse_core.h"

#define HCNSE_MIN_ORDER  1
#define HCNSE_MIN_ALLOC  (HCNSE_ALIGN_SIZE << HCNSE_MIN_ORDER)

#define HCNSE_SIZEOF_MEMNODE_T_ALIGN \
    hcnse_align_default(sizeof(hcnse_memnode_t))
#define HCNSE_SIZEOF_MEMPOOL_T_ALIGN \
    hcnse_align_default(sizeof(hcnse_pool_t))

/* 
 * Slot  0: size 4096
 * Slot  1: size 8192
 * Slot  2: size 12288
 * ...
 * Slot 19: size 81920
 */
#define HCNSE_MAX_POOL_SLOT  20


typedef struct hcnse_memnode_s hcnse_memnode_t;
typedef struct hcnse_cleanup_node_s hcnse_cleanup_node_t;

struct hcnse_memnode_s {
    hcnse_memnode_t *next;
    uint8_t *begin;
    uint8_t *first_avail;
    size_t size;
    size_t size_avail; /* To increase search speed */
};

struct hcnse_cleanup_node_s {
    hcnse_cleanup_node_t *next;
    void *data;
    hcnse_cleanup_handler_t handler;
};

struct hcnse_pool_s {
    hcnse_memnode_t *nodes[HCNSE_MAX_POOL_SLOT];
    hcnse_pool_t *parent;
    hcnse_pool_t *brother;
    hcnse_pool_t *child;

    hcnse_cleanup_node_t *cleanups;
    hcnse_cleanup_node_t *free_cleanups;
    /* hcnse_mutex_t mutex; */
};


static size_t
hcnse_align_allocation(size_t size)
{
    size_t align_size;

    align_size = hcnse_align_default(size);
    if (align_size < size) {
        return 0;
    }
    if (align_size < HCNSE_MIN_ALLOC) {
        align_size = HCNSE_MIN_ALLOC;
    }
    return align_size;
}

static size_t
hcnse_get_npages(size_t size)
{
    size_t total_size;

    total_size = size + HCNSE_SIZEOF_MEMNODE_T_ALIGN;
    if (total_size < HCNSE_PAGE_SIZE) {
        return 1;
    }
    else {
        return ((total_size / HCNSE_PAGE_SIZE) + 1);
    }
}

static hcnse_memnode_t *
hcnse_memnode_allocate_and_init(size_t size)
{
    hcnse_memnode_t *node;
    uint8_t *mem;

#if (HCNSE_HAVE_MMAP && HCNSE_POOL_USE_MMAP)
    mem = mmap(NULL, size, PROT_READ|PROT_WRITE,
                                MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        return NULL;
    }
#else
    mem = hcnse_malloc(size);
    if (mem == NULL) {
        return NULL;
    }
#endif

    node = (hcnse_memnode_t *) mem;

    /* Initialization of memnode */
    node->next = NULL;
    node->begin = mem + HCNSE_SIZEOF_MEMNODE_T_ALIGN;
    node->first_avail = node->begin;
    node->size = size - HCNSE_SIZEOF_MEMNODE_T_ALIGN;
    node->size_avail = node->size;

    return node;
}

static void
hcnse_pool_cleanup_run(hcnse_pool_t *pool)
{
    hcnse_cleanup_node_t *temp1, *temp2;

    temp1 = pool->cleanups;

    while (temp1) {

        temp2 = temp1;
        temp1 = temp1->next;

        temp2->handler(temp2->data);

        /* Reserve node */
        temp2->next = pool->free_cleanups;
        pool->free_cleanups = temp2;
    }

    pool->cleanups = NULL;
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
hcnse_pool_create1(hcnse_pool_t **newpool, size_t size, hcnse_pool_t *parent)
{
    hcnse_memnode_t *node;
    hcnse_pool_t *pool;
    size_t npages, index, align_size;

    align_size = hcnse_align_allocation(size);
    if (align_size == 0) {
        return HCNSE_FAILED;
    }

    align_size += HCNSE_SIZEOF_MEMPOOL_T_ALIGN;

    npages = hcnse_get_npages(align_size);
    index = npages - 1;
    if (index > HCNSE_MAX_POOL_SLOT) {
        hcnse_log_error1(HCNSE_LOG_ERROR, HCNSE_FAILED,
            "Initial pool size (%zu) out of range (0 - %zu)",
            size, HCNSE_MAX_POOL_SLOT * HCNSE_PAGE_SIZE);
        return HCNSE_FAILED;
    }

    node = hcnse_memnode_allocate_and_init(npages * HCNSE_PAGE_SIZE);
    if (!node) {
        return hcnse_get_errno();
    }

    pool = (hcnse_pool_t *) node->begin;
    hcnse_memset(pool, 0, sizeof(hcnse_pool_t));

    node->begin += HCNSE_SIZEOF_MEMPOOL_T_ALIGN;
    node->size -= HCNSE_SIZEOF_MEMPOOL_T_ALIGN;
    node->first_avail = node->begin;
    node->size_avail = node->size;

    /* Registaration of memnode */
    (pool->nodes)[index] = node;
    node->next = NULL;

    if (parent) {
        hcnse_pool_add_child(parent, pool);
    }

    *newpool = pool;
    return HCNSE_OK;
}

hcnse_pool_t *
hcnse_pool_create(size_t size, hcnse_pool_t *parent)
{
    hcnse_pool_t *pool;

    if (hcnse_pool_create1(&pool, size, parent) != HCNSE_OK) {
        return NULL;
    }
    return pool;
}

void *
hcnse_palloc(hcnse_pool_t *pool, size_t size)
{
    hcnse_memnode_t *node, *temp;
    hcnse_uint_t i;
    size_t align_size, npages, index;
    void *mem;

    node = NULL;
    mem = NULL;

    align_size = hcnse_align_allocation(size);
    if (!align_size) {
        goto failed;
    }

    /*
     * First see if there are any nodes in the area we know
     * our node will fit into
     */
    npages = hcnse_get_npages(align_size);
    index = npages - 1;

    /*
     * When the requested allocation is too large to fit into a block,
     * the request is forwarded to the system allocator and the returned
     * pointer is stored in the pool for further deallocation
     */
    if (index > HCNSE_MAX_POOL_SLOT) {
        mem = hcnse_malloc(align_size);
        if (mem) {
            hcnse_pool_cleanup_add(pool, mem, hcnse_free);
        }
        goto done;
    }

    /* Try to find suitable node */
    for (i = index; i < HCNSE_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            if (node->size_avail >= align_size) {
                node->size_avail -= align_size;
                mem = node->first_avail;
                node->first_avail += align_size;
                goto done;
            }
            node = node->next;
        }
    }

    /* If we haven't got a suitable node, allocate a new one */
    if (!node) {
        node = hcnse_memnode_allocate_and_init(npages * HCNSE_PAGE_SIZE);
        if (!node) {
            goto failed;
        }

        /* Registaration of memnode */
        temp = (pool->nodes)[index];
        (pool->nodes)[index] = node;
        node->next = temp;

        node->size_avail -= align_size;
        mem = node->first_avail;
        node->first_avail += align_size;
        goto done;
    }

done:
    return mem;
failed:
    return NULL;
}

void *
hcnse_pcalloc(hcnse_pool_t *pool, size_t size)
{
    void *mem;

    mem = hcnse_palloc(pool, size);
    if (mem) {
        hcnse_memset(mem, 0, size);
    }
    return mem;
}

void
hcnse_pool_cleanup_add1(hcnse_pool_t *pool, void *data,
    hcnse_cleanup_handler_t handler)
{
    hcnse_cleanup_node_t *node;

    if (pool->free_cleanups) {
        node = pool->free_cleanups;
        pool->free_cleanups = node->next;
    }
    else {
        node = hcnse_palloc(pool, sizeof(hcnse_cleanup_node_t));
    }
    node->data = data;
    node->handler = handler;
    node->next = pool->cleanups;
    pool->cleanups = node;
}

void
hcnse_pool_cleanup_remove1(hcnse_pool_t *pool, void *data,
    hcnse_cleanup_handler_t handler)
{
    hcnse_cleanup_node_t *node, *prev;

    node = pool->cleanups;

    while (node) {
        if (node->data == data && node->handler == handler) {
            break;
        }
        prev = node;
        node = node->next;
    }

    if (!node) {
        /* Not found */
        return;
    }

    if (pool->cleanups == node) {
        pool->cleanups = node->next;
    }
    else {
        prev->next = node->next;
    }

    /* Reserve node */
    node->next = pool->free_cleanups;
    pool->free_cleanups = node;
}

size_t
hcnse_pool_get_size(hcnse_pool_t *pool)
{
    hcnse_memnode_t *node;
    hcnse_uint_t i;
    size_t size;

    size = 0;

    for (i = 0; i < HCNSE_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            size += node->size;
            node = node->next;
        }
    }
    return size;
}

size_t
hcnse_pool_get_free_size(hcnse_pool_t *pool)
{
    hcnse_memnode_t *node;
    hcnse_uint_t i;
    size_t size;

    size = 0;

    for (i = 0; i < HCNSE_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            size += node->size_avail;
            node = node->next;
        }
    }
    return size;
}

size_t
hcnse_pool_get_total_size(hcnse_pool_t *pool)
{
    hcnse_memnode_t *node;
    hcnse_uint_t i;
    size_t size;

    size = 0;

    for (i = 0; i < HCNSE_MAX_POOL_SLOT; ++i) {
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
    hcnse_pool_t *temp1, *temp2;
    hcnse_memnode_t *node;
    hcnse_uint_t i;

    hcnse_pool_cleanup_run(pool);

    if (pool->child) {
        temp1 = pool->child;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            hcnse_pool_clean(temp2);
        }
    }

    for (i = 0; i < HCNSE_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            node->first_avail = node->begin;
            node->size_avail = node->size;
            node = node->next;
        }
    }
}

void
hcnse_pool_destroy(hcnse_pool_t *pool)
{
    hcnse_pool_t *temp1, *temp2;
    hcnse_memnode_t *node, *temp;
    hcnse_uint_t i;

    hcnse_pool_cleanup_run(pool);

    if (pool->child) {
        temp1 = pool->child;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            hcnse_pool_destroy(temp2);
        }
    }

    for (i = 0; i < HCNSE_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            temp = node;
            node = node->next;
#if (HCNSE_HAVE_MMAP && HCNSE_POOL_USE_MMAP)
            munmap(temp, HCNSE_PAGE_SIZE * (i + 1));
#else
            hcnse_free(temp);
#endif
        }
    }
}
