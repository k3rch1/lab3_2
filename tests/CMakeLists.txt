add_subdirectory(
    ${CMAKE_CURRENT_SOURCE_DIR}/../googletest
    googletest-build
)

enable_testing()

add_executable(
    tests
    hash_table_tests.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/subs/lab5/include
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(
    tests
    GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(tests)