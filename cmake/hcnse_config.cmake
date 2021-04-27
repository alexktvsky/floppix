include(CheckCSourceCompiles)

check_c_source_compiles("
int main(void) {
#if defined(__linux__) || defined(__gnu_linux__)
#elif
    generete_error;
#endif
}" HCNSE_LINUX)

check_c_source_compiles("
int main(void) {
#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#elif
    generete_error();
#endif
}" HCNSE_FREEBSD)

check_c_source_compiles("
int main(void) {
#if defined(__APPLE__) || defined(__MACH__)
#elif
    generete_error();
#endif
}" HCNSE_DARWIN)

check_c_source_compiles("
int main(void) {
#if defined(__sun) && defined(__SVR4)
#elif
    generete_error();
#endif
}" HCNSE_SOLARIS)

check_c_source_compiles("
int main(void) {
#if defined(_WIN32) || defined(__WIN32__)
#elif
    generete_error();
#endif
}" HCNSE_WIN32)

if (NOT(HCNSE_LINUX OR
        HCNSE_FREEBSD OR
        HCNSE_DARWIN OR
        HCNSE_SOLARIS OR
        HCNSE_WIN32))
    message(FATAL_ERROR "Not supported platform")
endif()

if (HCNSE_LINUX OR HCNSE_FREEBSD OR HCNSE_DARWIN OR HCNSE_SOLARIS)
    set(HCNSE_POSIX 1)
endif()

check_c_source_compiles("
#include <stddef.h>
#include <sys/mman.h>
int main(void) {
    mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    return 0;
}" HCNSE_HAVE_MMAP)

check_c_source_compiles("
#include <dirent.h>
#include <sys/types.h>
int main(void) {
    DIR *dir = opendir(\"/\");
    struct dirent *dirent = readdir(dir);
    dirent->d_type;
    return 0;
}" HCNSE_HAVE_D_TYPE)

set(CMAKE_REQUIRED_LINK_OPTIONS -lpthread)
check_c_source_compiles("
#include <semaphore.h>
int main(void) {
    sem_t s;
    sem_init(&s, 0, 1);
    return 0;
}" HCNSE_HAVE_POSIX_SEM)

check_c_source_compiles("
#include <dispatch/dispatch.h>
int main(void) {
    dispatch_semaphore_create(1);
    return 0;
}" HCNSE_HAVE_GCD_SEM)

check_c_source_compiles("
#include <stddef.h>
#include <sys/select.h>
int main(void) {
    fd_set rfds;
    fd_set wfds;
    struct timeval timeout;
    select(1, &rfds, &wfds, NULL, &timeout);
    return 0;
}" HCNSE_HAVE_SELECT)

check_c_source_compiles("
#include <sys/epoll.h>
int main(void) {
    epoll_create1(0);
    return 0;
}" HCNSE_HAVE_EPOLL)

check_c_source_compiles("
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
int main(void) {
    kqueue();
    return 0;
}" HCNSE_HAVE_KQUEUE)

set(CMAKE_REQUIRED_LINK_OPTIONS -lssl -lcrypto)
check_c_source_compiles("
#include <openssl/opensslv.h>
#include <openssl/crypto.h>
int main(void) {
    OpenSSL_version(OPENSSL_VERSION);
    return 0;
}" HCNSE_HAVE_OPENSSL)

configure_file(
    "cmake/hcnse_config.h.in"
    "${PROJECT_BINARY_DIR}/hcnse_config.h"
)
