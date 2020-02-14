#ifndef INCLUDED_MEMORY_H
#define INCLUDED_MEMORY_H

#if defined(__GNUC__)
#define memory_barrier() __sync_synchronize()
#elif defined(__sparc) || defined(__sparc__)
#define memory_barrier() __asm (".volatile"); __asm (".nonvolatile")
/* TODO: etc */
#endif


void explicit_memzero(void *buf, size_t n);

#endif /* INCLUDED_MEMORY_H */
