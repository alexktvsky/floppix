add_test(${PROJECT_NAME}_test "./test")

target_include_directories(${PROJECT_NAME}
PRIVATE
    include
    unit
    ${PROJECT_BINARY_DIR}
    ../src
    ../src/algo
    ../src/core
    ../src/system
    ../src/system/event
    ../src/system/net
    ../src/system/os
    ../src/system/sync
    ../src/util
    ../${PROJECT_BINARY_DIR}
)

target_sources(${PROJECT_NAME}
PRIVATE
    unit/test.c
    unit/fpx.test.unit.stubs.c
    unit/fpx.test.unit.pool.c
)

target_link_options(${PROJECT_NAME}
PRIVATE
    -Wl,--wrap=fpx_malloc1
    -Wl,--wrap=fpx_palloc1
)

# add_dependencies(${PROJECT_NAME}_test ${PROJECT_NAME})
