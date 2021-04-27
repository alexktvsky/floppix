option(OPTION_POOL_THREAD_SAFETY "Pool is thread safety" ON)
option(OPTION_POOL_USES_MMAP "Pool uses mmap" OFF)
option(OPTION_LOGGER_IS_PROC "Logger implements as process not thread" OFF)


if (OPTION_POOL_THREAD_SAFETY)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_POOL_THREAD_SAFETY)
endif()

if (OPTION_POOL_USES_MMAP)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_POOL_USES_MMAP)
endif()

if (OPTION_LOGGER_IS_PROC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HCNSE_LOGGER_IS_PROC)
endif()
