include(CheckCSourceCompiles)

check_c_source_compiles("
int main(void) {
#if defined(__linux__) || defined(__gnu_linux__)
#elif
    generete_error;
#endif
}" FPX_LINUX)

check_c_source_compiles("
int main(void) {
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#elif
    generete_error();
#endif
}" FPX_FREEBSD)

check_c_source_compiles("
int main(void) {
#if defined(__APPLE__) || defined(__MACH__)
#elif
    generete_error();
#endif
}" FPX_DARWIN)

check_c_source_compiles("
int main(void) {
#if defined(__sun) && defined(__SVR4)
#elif
    generete_error();
#endif
}" FPX_SOLARIS)

check_c_source_compiles("
int main(void) {
#if defined(_WIN32) || defined(__WIN32__)
#elif
    generete_error();
#endif
}" FPX_WIN32)

if (NOT(FPX_LINUX OR
        FPX_FREEBSD OR
        FPX_DARWIN OR
        FPX_SOLARIS OR
        FPX_WIN32))
    message(FATAL_ERROR "Not supported platform")
endif()

if (FPX_LINUX OR FPX_FREEBSD OR FPX_DARWIN OR FPX_SOLARIS)
    set(FPX_POSIX 1)
endif()

check_c_source_compiles("
#include <stdbool.h>
int main(void) {
    sizeof(bool);
    true;
    false;
    return 0;
}" FPX_HAVE_STDBOOL_H)

check_c_source_compiles("
#include <stdint.h>
int main(void) {
    sizeof(int8_t);
    sizeof(int16_t);
    sizeof(int32_t);
    sizeof(int64_t);
    sizeof(uint8_t);
    sizeof(uint16_t);
    sizeof(uint32_t);
    sizeof(uint64_t);
    sizeof(intptr_t);
    sizeof(uintptr_t);
    return 0;
}" FPX_HAVE_STDINT_H)

check_c_source_compiles("
#include <stddef.h>
int main(void) {
    sizeof(size_t);
    return 0;
}" FPX_HAVE_SIZE_T)

check_c_source_compiles("
#include <unistd.h>
int main(void) {
    sizeof(ssize_t);
    return 0;
}" FPX_HAVE_SSIZE_T)

check_c_source_compiles("
#include <stddef.h>
int main(void) {
    sizeof(ptrdiff_t);
    return 0;
}" FPX_HAVE_PTRDIFF_T)

check_c_source_compiles("
#include <stddef.h>
#include <sys/mman.h>
int main(void) {
    mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    return 0;
}" FPX_HAVE_MMAP)

check_c_source_compiles("
#include <dirent.h>
#include <sys/types.h>
int main(void) {
    DIR *dir = opendir(\"/\");
    struct dirent *dirent = readdir(dir);
    dirent->d_type;
    return 0;
}" FPX_HAVE_D_TYPE)

check_c_source_compiles("
#include <unistd.h>
int main(void) {
    sysconf(_SC_PAGESIZE);
    return 0;
}" FPX_HAVE_SC_PAGESIZE)

check_c_source_compiles("
#include <unistd.h>
int main(void) {
    sysconf(_SC_NPROCESSORS_ONLN);
    return 0;
}" FPX_HAVE_SC_NPROC)

set(CMAKE_REQUIRED_LINK_OPTIONS -lpthread)
check_c_source_compiles("
#include <semaphore.h>
int main(void) {
    sem_t s;
    return 0;
}" FPX_HAVE_POSIX_SEM)

check_c_source_compiles("
#include <dispatch/dispatch.h>
int main(void) {
    dispatch_semaphore_create(1);
    return 0;
}" FPX_HAVE_GCD_SEM)

check_c_source_compiles("
#include <stddef.h>
#include <sys/select.h>
int main(void) {
    fd_set rfds;
    fd_set wfds;
    struct timeval timeout;
    select(1, &rfds, &wfds, NULL, &timeout);
    return 0;
}" FPX_HAVE_SELECT)

check_c_source_compiles("
#include <sys/epoll.h>
int main(void) {
    epoll_create1(0);
    return 0;
}" FPX_HAVE_EPOLL)

check_c_source_compiles("
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
int main(void) {
    kqueue();
    return 0;
}" FPX_HAVE_KQUEUE)

set(CMAKE_REQUIRED_LINK_OPTIONS -lssl -lcrypto)
check_c_source_compiles("
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
int main(void) {
    OpenSSL_version(OPENSSL_VERSION);
    return 0;
}" FPX_HAVE_OPENSSL)

configure_file(
    "cmake/fpx_system_config.h.in"
    "${PROJECT_BINARY_DIR}/fpx_system_config.h"
)
