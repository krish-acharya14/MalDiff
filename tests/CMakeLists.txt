add_executable(mede_tests
    test_config_manager.cpp
    test_storage_manager.cpp
    test_sample.cpp
    test_string_utils.cpp
)

target_link_libraries(mede_tests
    PRIVATE
        mede_core
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(mede_tests)
