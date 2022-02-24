#include "floppix/system/pool.h"
#include "floppix/system/memory.h"
#include "floppix/system/types.h"
#include "floppix/system/thread.h"
#include "floppix/system/sync/mutex.h"
#include "floppix/core/log.h"

#define FPX_MIN_ORDER              1
#define FPX_MIN_ALLOC              (FPX_ALIGN_SIZE << FPX_MIN_ORDER)

#define FPX_SIZEOF_MEMNODE_T_ALIGN fpx_align_default(sizeof(fpx_memnode_t))
#define FPX_SIZEOF_MEMPOOL_T_ALIGN fpx_align_default(sizeof(fpx_pool_t))
#define FPX_SIZEOF_MUTEX_T_ALIGN   fpx_align_default(sizeof(fpx_mutex_t))

/*
 * Slot  0: size 4096
 * Slot  1: size 8192
 * Slot  2: size 12288
 * ...
 * Slot 19: size 81920
 */
#define FPX_MAX_POOL_SLOT          20

typedef struct fpx_memnode_s fpx_memnode_t;
typedef struct fpx_cleanup_node_s fpx_cleanup_node_t;

struct fpx_memnode_s {
    fpx_memnode_t *next;
    uint8_t *begin;
    uint8_t *first_avail;
    size_t size;
    size_t size_avail; /* To increase search speed */
    size_t dealloc_size;
};

struct fpx_cleanup_node_s {
    fpx_cleanup_node_t *next;
    void *data;
    fpx_cleanup_handler_t handler;
};

struct fpx_pool_s {
    fpx_memnode_t *nodes[FPX_MAX_POOL_SLOT];
    fpx_pool_t *parent;
    fpx_pool_t *brother;
    fpx_pool_t *child;

    fpx_cleanup_node_t *cleanups;
    fpx_cleanup_node_t *free_cleanups;

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_t *mutex;
    fpx_thread_handle_t owner;
#endif
};

static size_t
fpx_align_allocation(size_t size)
{
    size_t align_size;

    align_size = fpx_align_default(size);
    if (align_size < size) {
        return 0;
    }
    if (align_size < FPX_MIN_ALLOC) {
        align_size = FPX_MIN_ALLOC;
    }
    return align_size;
}

static size_t
fpx_get_npages(size_t size)
{
    size_t total_size;

    total_size = size + FPX_SIZEOF_MEMNODE_T_ALIGN;
    if (total_size < FPX_PAGE_SIZE) {
        return 1;
    }
    else {
        return ((total_size / FPX_PAGE_SIZE) + 1);
    }
}

static fpx_memnode_t *
fpx_memnode_allocate(size_t size)
{
    fpx_memnode_t *node;
    uint8_t *mem;

#if (FPX_HAVE_MMAP && FPX_POOL_USES_MMAP)
    mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0);
    if (mem == MAP_FAILED) {
        return NULL;
    }
#else
    mem = fpx_malloc(size);
    if (mem == NULL) {
        return NULL;
    }
#endif

    node = (fpx_memnode_t *) mem;

    /* Initialization of memnode */
    node->next = NULL;
    node->begin = mem + FPX_SIZEOF_MEMNODE_T_ALIGN;
    node->first_avail = node->begin;
    node->size = size - FPX_SIZEOF_MEMNODE_T_ALIGN;
    node->size_avail = node->size;
    node->dealloc_size = size;

    return node;
}

static void
fpx_memnode_deallocate(fpx_memnode_t *node)
{
#if (FPX_HAVE_MMAP && FPX_POOL_USES_MMAP)
    munmap(node, node->dealloc_size);
#else
    fpx_free(node);
#endif
}

static void
fpx_pool_cleanup_run_all(fpx_pool_t *pool)
{
    fpx_cleanup_node_t *temp1, *temp2;

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
fpx_pool_add_child(fpx_pool_t *parent, fpx_pool_t *new_child)
{
    fpx_pool_t *temp;

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

fpx_err_t
fpx_pool_create(fpx_pool_t **pool, size_t size, fpx_pool_t *parent)
{
    fpx_memnode_t *node;
    fpx_pool_t *new_pool;
    size_t npages, index, align_size;

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_t *mutex;
#endif
    fpx_err_t err;

    node = NULL;

    align_size = fpx_align_allocation(size);
    if (align_size == 0) {
        err = FPX_FAILED;
        goto failed;
    }

    align_size += FPX_SIZEOF_MEMPOOL_T_ALIGN;

    npages = fpx_get_npages(align_size);
    index = npages - 1;
    if (index > FPX_MAX_POOL_SLOT) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_FAILED,
            "Initial pool size (%zu) out of range (0 - %zu)", size,
            FPX_MAX_POOL_SLOT * FPX_PAGE_SIZE);
        err = FPX_FAILED;
        goto failed;
    }

    node = fpx_memnode_allocate(npages * FPX_PAGE_SIZE);
    if (!node) {
        err = fpx_get_errno();
        goto failed;
    }

    /* Reserve memory for service info */
    new_pool = (fpx_pool_t *) node->begin;
    fpx_memset(new_pool, 0, sizeof(fpx_pool_t));
    node->begin += FPX_SIZEOF_MEMPOOL_T_ALIGN;
    node->size -= FPX_SIZEOF_MEMPOOL_T_ALIGN;

#if (FPX_POOL_THREAD_SAFETY)

    if (parent) {
        mutex = parent->mutex;
    }
    else {
        mutex = (fpx_mutex_t *) node->begin;
        node->begin += FPX_SIZEOF_MUTEX_T_ALIGN;
        node->size -= FPX_SIZEOF_MUTEX_T_ALIGN;

        err = fpx_mutex_init(mutex, FPX_MUTEX_SHARED | FPX_MUTEX_RECURSIVE);
        if (err != FPX_OK) {
            goto failed;
        }
    }

    new_pool->mutex = mutex;
    new_pool->owner = fpx_thread_current_handle();

#endif

    /* Modify memnode service info */
    node->first_avail = node->begin;
    node->size_avail = node->size;

    /* Registaration of memnode */
    (new_pool->nodes)[index] = node;
    node->next = NULL;

    if (parent) {
        fpx_pool_add_child(parent, new_pool);
    }

    *pool = new_pool;

    return FPX_OK;

failed:
    if (node) {
        fpx_memnode_deallocate(node);
    }

    return err;
}

void *
fpx_palloc1(const char *filename, int line, fpx_pool_t *pool, size_t size)
{
    fpx_memnode_t *node, *temp;
    uint i;
    size_t align_size, npages, index;
    void *mem;

    node = NULL;
    mem = NULL;

    align_size = fpx_align_allocation(size);
    if (!align_size) {
        goto failed;
    }

    /*
     * First see if there are any nodes in the area we know
     * our node will fit into
     */
    npages = fpx_get_npages(align_size);
    index = npages - 1;

    /*
     * When the requested allocation is too large to fit into a block,
     * the request is forwarded to the system allocator and the returned
     * pointer is stored in the pool for further deallocation
     */
    if (index > FPX_MAX_POOL_SLOT) {
        mem = fpx_malloc(align_size);
        if (mem) {
            fpx_pool_cleanup_add(pool, mem, fpx_free);
        }
        goto done;
    }

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_lock(pool->mutex);
#endif

    /* Try to find suitable node */
    for (i = index; i < FPX_MAX_POOL_SLOT; ++i) {
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
        node = fpx_memnode_allocate(npages * FPX_PAGE_SIZE);
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
#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_unlock(pool->mutex);
#endif
    fpx_log_debug1(FPX_OK, "palloc %p %zu in %s:" FPX_FMT_UINT_T, mem, size,
        filename, line);
    return mem;

failed:
#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_unlock(pool->mutex);
#endif
    return NULL;
}

void *
fpx_pcalloc1(const char *filename, int line, fpx_pool_t *pool, size_t size)
{
    void *mem;

    mem = fpx_palloc1(filename, line, pool, size);
    if (mem) {
        fpx_memset(mem, 0, size);
    }
    return mem;
}

void
fpx_pool_cleanup_add1(fpx_pool_t *pool, void *data,
    fpx_cleanup_handler_t handler)
{
    fpx_cleanup_node_t *node;

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_lock(pool->mutex);
#endif

    if (pool->free_cleanups) {
        node = pool->free_cleanups;
        pool->free_cleanups = node->next;
    }
    else {
        node = fpx_palloc(pool, sizeof(fpx_cleanup_node_t));
    }
    node->data = data;
    node->handler = handler;
    node->next = pool->cleanups;
    pool->cleanups = node;

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_unlock(pool->mutex);
#endif
}

void
fpx_pool_cleanup_run1(fpx_pool_t *pool, void *data,
    fpx_cleanup_handler_t handler)
{
    fpx_cleanup_node_t *node, *prev;

    node = pool->cleanups;

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_lock(pool->mutex);
#endif

    while (node) {
        if (node->data == data && node->handler == handler) {
            node->handler(node->data);
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

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_unlock(pool->mutex);
#endif
}

void
fpx_pool_cleanup_remove1(fpx_pool_t *pool, void *data,
    fpx_cleanup_handler_t handler)
{
    fpx_cleanup_node_t *node, *prev;

    node = pool->cleanups;

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_lock(pool->mutex);
#endif

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

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_unlock(pool->mutex);
#endif
}

size_t
fpx_pool_get_size(fpx_pool_t *pool)
{
    fpx_memnode_t *node;
    uint i;
    size_t size;

    size = 0;

    for (i = 0; i < FPX_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            size += node->size;
            node = node->next;
        }
    }
    return size;
}

size_t
fpx_pool_get_free_size(fpx_pool_t *pool)
{
    fpx_memnode_t *node;
    uint i;
    size_t size;

    size = 0;

    for (i = 0; i < FPX_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            size += node->size_avail;
            node = node->next;
        }
    }
    return size;
}

size_t
fpx_pool_get_total_size(fpx_pool_t *pool)
{
    fpx_memnode_t *node;
    uint i;
    size_t size;

    size = 0;

    for (i = 0; i < FPX_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            size += FPX_PAGE_SIZE * (i + 1);
            node = node->next;
        }
    }
    return size;
}

void
fpx_pool_clear(fpx_pool_t *pool)
{
    fpx_pool_t *temp1, *temp2;
    fpx_memnode_t *node;
    uint i;

#if (FPX_POOL_THREAD_SAFETY)
    if (!fpx_thread_equal(pool->owner, fpx_thread_current_handle())) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_FAILED,
            "Unexpected access to a pool from a thread with tid " FPX_FMT_TID_T,
            fpx_thread_current_tid());
        return;
    }

    fpx_mutex_lock(pool->mutex);
#endif

    fpx_pool_cleanup_run_all(pool);

    if (pool->child) {
        temp1 = pool->child;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            fpx_pool_clear(temp2);
        }
    }

    for (i = 0; i < FPX_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            node->first_avail = node->begin;
            node->size_avail = node->size;
            node = node->next;
        }
    }

#if (FPX_POOL_THREAD_SAFETY)
    fpx_mutex_unlock(pool->mutex);
#endif
}

void
fpx_pool_destroy(fpx_pool_t *pool)
{
    fpx_pool_t *temp1, *temp2;
    fpx_memnode_t *node, *temp;
    uint i;

#if (FPX_POOL_THREAD_SAFETY)
    if (!fpx_thread_equal(pool->owner, fpx_thread_current_handle())) {
        fpx_log_error1(FPX_LOG_ERROR, FPX_FAILED,
            "Unexpected access to a pool from a thread with tid " FPX_FMT_TID_T,
            fpx_thread_current_tid());
        return;
    }
#endif

    fpx_pool_cleanup_run_all(pool);

    if (pool->child) {
        temp1 = pool->child;
        while (temp1) {
            temp2 = temp1;
            temp1 = temp1->brother;
            fpx_pool_destroy(temp2);
        }
    }

    for (i = 0; i < FPX_MAX_POOL_SLOT; ++i) {
        node = (pool->nodes)[i];
        while (node) {
            temp = node;
            node = node->next;
            fpx_memnode_deallocate(temp);
        }
    }
}
