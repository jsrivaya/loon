find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(loon_tests
    test_lru.cpp
    test_redis_list.cpp
    test_ring_buffer.cpp
)
target_link_libraries(loon_tests PRIVATE loon GTest::gtest_main)
gtest_discover_tests(loon_tests)
