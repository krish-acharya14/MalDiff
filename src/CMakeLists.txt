add_library(mede_core STATIC
    core/Application.cpp
    config/ConfigManager.cpp
    logging/Logger.cpp
    storage/StorageManager.cpp
    utils/FileUtils.cpp
    utils/StringUtils.cpp
    models/AnalysisStatus.cpp
    pipeline/FileValidator.cpp
    pipeline/BinaryLoader.cpp
    pipeline/MagicDetector.cpp
    pipeline/HashEngine.cpp
    pipeline/MetadataExtractor.cpp
    pipeline/SampleManager.cpp
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

set_target_properties(mede_cli PROPERTIES OUTPUT_NAME "mede")
