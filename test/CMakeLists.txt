add_test(${PROJECT_NAME}_test "./test")

target_include_directories(${PROJECT_NAME}
PRIVATE
    include
    ../include
    ../${PROJECT_BINARY_DIR}
    ${PROJECT_BINARY_DIR}
)

target_sources(${PROJECT_NAME}
PRIVATE
    unit/test.c
    unit/hcnse_stubs.c
    unit/test_hcnse_pool.c
)

target_link_options(${PROJECT_NAME}
PRIVATE
    -Wl,--wrap=hcnse_malloc
    -Wl,--wrap=hcnse_palloc
)

# add_dependencies(${PROJECT_NAME}_test ${PROJECT_NAME})
