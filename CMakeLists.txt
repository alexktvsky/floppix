cmake_minimum_required(VERSION 3.14.0)

project(hcnse)

add_executable(${PROJECT_NAME})

include(cmake/hcnse_options.cmake)
include(cmake/hcnse_config.cmake)

target_include_directories(${PROJECT_NAME}
PRIVATE
    include
    include/container
    include/core
    include/system
    include/system/net
    include/system/os
    include/system/sync
    include/type
    include/util
    ${PROJECT_BINARY_DIR}
)

target_sources(${PROJECT_NAME}
PRIVATE
    src/container/hcnse.container.array.c
    src/container/hcnse.container.list.c
    src/core/hcnse.core.module.c
    src/core/hcnse.core.parse.c
    src/core/hcnse.core.listen.c
    src/core/hcnse.core.cycle.c
    src/core/hcnse.core.server.c
    src/core/hcnse.core.log.c
    src/core/hcnse.core.connection.c
    src/core/hcnse.core.command.c
    src/system/hcnse.system.process.c
    src/system/net/hcnse.system.net.sockaddr.c
    src/system/net/hcnse.system.net.socket.c
    src/system/net/hcnse.system.net.sockopt.c
    src/system/hcnse.system.pool.c
    src/system/hcnse.system.thread.c
    src/system/hcnse.system.dso.c
    src/system/hcnse.system.time.c
    src/system/hcnse.system.memory.c
    src/system/hcnse.system.errno.c
    src/system/hcnse.system.signal.c
    src/system/hcnse.system.filesys.c
    src/system/hcnse.system.cpuinfo.c
    src/system/sync/hcnse.system.sync.semaphore.c
    src/system/sync/hcnse.system.sync.mutex.c
    src/util/hcnse.util.string.c
    src/core/main.c
)

if (NOT BUILD_TESTING)
    target_sources(${PROJECT_NAME} PRIVATE src/core/main.c)
endif()

target_compile_options(${PROJECT_NAME}
PRIVATE
    -Wall
    -Wextra
    -Wno-uninitialized
)

target_compile_features(${PROJECT_NAME} PRIVATE c_std_99)
target_compile_definitions(${PROJECT_NAME}
PRIVATE
    HCNSE_COMPILER="${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}"
    HCNSE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}"
)

if (NOT CMAKE_BUILD_TYPE)
    set (build_type release)
else()
    string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
endif()

if (build_type STREQUAL debug)
    target_compile_options(${PROJECT_NAME} PRIVATE -g3 -O0 -ggdb3)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        HCNSE_DEBUG
        HCNSE_HAVE_VALGRIND
    )
elseif (build_type STREQUAL release)
    target_compile_options(${PROJECT_NAME} PRIVATE -g0 -O3)
endif()


# option(OPTION_BUILD_DOCS "Build documentation" OFF)

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_REQUIRED_LIBRARIES}
)

if (HCNSE_POSIX)
    target_link_libraries(${PROJECT_NAME} pthread)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
elseif(HCNSE_WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()

enable_testing()

if (BUILD_TESTING)
    add_subdirectory(test)
endif()
