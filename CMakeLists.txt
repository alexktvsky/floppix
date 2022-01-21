cmake_minimum_required(VERSION 3.14.0)

project(floppix)

add_executable(${PROJECT_NAME})

include(cmake/fpx_options.cmake)
include(cmake/fpx_config.cmake)

target_include_directories(${PROJECT_NAME}
PRIVATE
    include
    ${PROJECT_BINARY_DIR}
)

target_sources(${PROJECT_NAME}
PRIVATE
    src/algo/array.c
    src/algo/list.c
    src/core/module.c
    src/core/parse.c
    src/core/listen.c
    src/core/cycle.c
    src/core/server.c
    src/core/log.c
    src/core/connection.c
    src/core/command.c
    src/system/process.c
    src/system/net/sockaddr.c
    src/system/net/socket.c
    src/system/net/sockopt.c
    src/system/pool.c
    src/system/thread.c
    src/system/dso.c
    src/system/time.c
    src/system/memory.c
    src/system/errno.c
    src/system/signal.c
    src/system/filesys.c
    src/system/cpuinfo.c
    src/system/sync/semaphore.c
    src/system/sync/mutex.c
    src/util/string.c
)

if (NOT BUILD_TESTING)
    target_sources(${PROJECT_NAME} PRIVATE src/core/main.c)
endif()

target_compile_options(${PROJECT_NAME}
PRIVATE
    -Wall
    -Wextra
    -pedantic-errors # -pedantic
)

target_compile_features(${PROJECT_NAME} PRIVATE c_std_99)
target_compile_definitions(${PROJECT_NAME}
PRIVATE
    FPX_COMPILER="${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}"
    FPX_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}"
)

if (NOT CMAKE_BUILD_TYPE)
    set (build_type release)
else()
    string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
endif()

if (build_type STREQUAL debug)
    target_compile_options(${PROJECT_NAME} PRIVATE -g3 -O0 -ggdb3)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        FPX_DEBUG
        FPX_HAVE_VALGRIND
    )
elseif (build_type STREQUAL release)
    target_compile_options(${PROJECT_NAME} PRIVATE -g0 -O3)
endif()


# option(OPTION_BUILD_DOCS "Build documentation" OFF)

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_REQUIRED_LIBRARIES}
)

if (FPX_POSIX)
    target_link_libraries(${PROJECT_NAME} pthread)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
elseif(FPX_WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()

enable_testing()

if (BUILD_TESTING)
    add_subdirectory(test)
endif()
