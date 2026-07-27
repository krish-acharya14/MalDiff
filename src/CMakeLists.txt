add_library(mede_core STATIC
    core/Application.cpp
    config/ConfigManager.cpp
    storage/StorageManager.cpp
    utils/FileUtils.cpp
    utils/StringUtils.cpp
    utils/Logger.cpp
    analysis/ByteReader.cpp
    analysis/EntropyEngine.cpp
    analysis/ImportParser.cpp
    analysis/PEParser.cpp
    analysis/ReportGenerator.cpp
    analysis/StringExtractor.cpp
    analysis/AnalysisPipeline.cpp
)

target_include_directories(mede_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(mede_core
    PUBLIC
        mede_compiler_flags
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        OpenSSL::Crypto
)

add_executable(mede_cli
    main.cpp
    cli/CliHandler.cpp
)

target_link_libraries(mede_cli
    PRIVATE
        mede_core
        CLI11::CLI11
)

set_target_properties(mede_cli PROPERTIES OUTPUT_NAME "maldiff")
