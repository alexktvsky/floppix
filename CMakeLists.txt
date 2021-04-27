cmake_minimum_required(VERSION 3.14.0)

project(hcnse)

add_executable(${PROJECT_NAME})

include(cmake/hcnse_options.cmake)
include(cmake/hcnse_config.cmake)

target_include_directories(${PROJECT_NAME}
PRIVATE
    include
    ${PROJECT_BINARY_DIR}
)

target_sources(${PROJECT_NAME}
PRIVATE
    src/core/hcnse_array.c
    src/core/hcnse_command.c
    src/core/hcnse_connection.c
    src/core/hcnse_core.c
    src/core/hcnse_cycle.c
    src/core/hcnse_list.c
    src/core/hcnse_listen.c
    src/core/hcnse_log.c
    src/core/hcnse_module.c
    src/core/hcnse_parse.c
    src/core/hcnse_pool.c
    src/core/hcnse_string.c
    src/os/hcnse_memory.c
    src/os/hcnse_filesys.c
    src/os/hcnse_errno.c
    src/os/hcnse_sockaddr.c
    src/os/hcnse_socket.c
    src/os/hcnse_sockopt.c
    src/os/hcnse_time.c
    src/os/hcnse_thread.c
    src/os/hcnse_mutex.c
    src/os/hcnse_semaphore.c
    src/os/hcnse_cpuinfo.c
    src/os/hcnse_dso.c
    src/os/hcnse_process.c
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
    target_compile_options(${PROJECT_NAME}
    PRIVATE
        -g0 -O3
    )
    target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        HCNSE_DEBUG
        HCNSE_HAVE_VALGRIND
    )
elseif (build_type STREQUAL release)
    target_compile_options(${PROJECT_NAME}
    PRIVATE
        -g3 -O0 -ggdb3
    )
endif()


# option(OPTION_BUILD_DOCS "Build documentation" OFF)

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_REQUIRED_LIBRARIES}
    ${ALL_LIBRARIES}
)

string(TOLOWER ${CMAKE_SYSTEM_NAME} system_name)
if (system_name STREQUAL windows)
    target_link_libraries(${PROJECT_NAME} ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} pthread)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
endif()

enable_testing()

if (BUILD_TESTING)
    add_subdirectory(test)
endif()
