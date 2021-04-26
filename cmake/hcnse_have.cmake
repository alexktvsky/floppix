include(CheckCSourceCompiles)

check_c_source_compiles("
#include <stddef.h>
#include <sys/mman.h>
int main(void) {
    mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    return 0;
}" HCNSE_HAVE_MMAP)
if (HCNSE_HAVE_MMAP)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_HAVE_MMAP)
endif()

set(CMAKE_REQUIRED_LINK_OPTIONS -lpthread)
check_c_source_compiles("
#include <semaphore.h>
int main(void) {
    sem_t s;
    sem_init(&s, 0, 1);
    return 0;
}" HCNSE_HAVE_POSIX_SEM)
if (HCNSE_HAVE_POSIX_SEM)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_HAVE_POSIX_SEM)
endif()

check_c_source_compiles("
#include <dispatch/dispatch.h>
int main(void) {
    dispatch_semaphore_create(1);
    return 0;
}" HCNSE_HAVE_GCD_SEM)
if (HCNSE_HAVE_GCD_SEM)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_HAVE_GCD_SEM)
endif()

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
if (HCNSE_HAVE_SELECT)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_HAVE_SELECT)
endif()

check_c_source_compiles("
#include <sys/epoll.h>
int main(void) {
    epoll_create1(0);
    return 0;
}" HCNSE_HAVE_EPOLL)
if (HCNSE_HAVE_EPOLL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_HAVE_EPOLL)
endif()

check_c_source_compiles("
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
int main(void) {
    kqueue();
    return 0;
}" HCNSE_HAVE_KQUEUE)
if (HCNSE_HAVE_KQUEUE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_HAVE_KQUEUE)
endif()

check_c_source_compiles("
#include <dirent.h>
#include <sys/types.h>
int main(void) {
    DIR *dir = opendir(\"/\");
    struct dirent *dirent = readdir(dir);
    dirent->d_type;
    return 0;
}" HCNSE_HAVE_D_TYPE)
if (HCNSE_HAVE_D_TYPE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_HAVE_D_TYPE)
endif()
