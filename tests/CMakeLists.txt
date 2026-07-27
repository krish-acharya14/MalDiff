add_executable(mede_tests
    test_config_manager.cpp
    test_storage_manager.cpp
    test_string_utils.cpp
    analysis/test_entropy.cpp
    analysis/test_string_extractor.cpp
    analysis/test_pe_parser.cpp
    analysis/test_import_parser.cpp
    analysis/test_report.cpp
    analysis/test_pipeline.cpp
)

target_link_libraries(mede_tests
    PRIVATE
        mede_core
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(mede_tests)
