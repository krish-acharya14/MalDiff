cmake_minimum_required(VERSION 3.22)

project(
    MalwareEvolutionDiffEngine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "MalwareEvolutionDiffEngine version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(mede ${SOURCES})

target_include_directories(mede
    PRIVATE
        include
)

target_link_libraries(mede
    PRIVATE
        CLI11::CLI11
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        Threads::Threads
)

if(MSVC)
    target_compile_options(mede PRIVATE /W4 /permissive-)
else()
    target_compile_options(mede PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

set_target_properties(mede PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)