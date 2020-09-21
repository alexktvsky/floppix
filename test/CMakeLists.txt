
add_executable(${PROJECT_NAME}_test)
add_test(${PROJECT_NAME}_test "./test")

target_include_directories(${PROJECT_NAME}_test
PRIVATE
    ../include
)

target_sources(${PROJECT_NAME}_test
PRIVATE
    hcnse_test.c
    hcnse_stubs.c
    test_hcnse_pool.c
)

target_link_options(${PROJECT_NAME}_test
PRIVATE
    -Wl,--wrap=hcnse_malloc
    -Wl,--wrap=hcnse_palloc
)

# add_dependencies(${PROJECT_NAME}_test ${PROJECT_NAME})
