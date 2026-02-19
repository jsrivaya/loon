cmake_minimum_required(VERSION 3.20)
project(loon VERSION 0.1.0 LANGUAGES CXX)

# Header-only library target
add_library(loon INTERFACE)
target_include_directories(loon
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Alias for namespaced usage
add_library(loon::loon ALIAS loon)

# Generate and install config package
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/loonConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/loonConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/loonConfig.cmake"
    INSTALL_DESTINATION lib/cmake/loon
)

install(
    TARGETS loon
    EXPORT loonTargets
)

install(
    DIRECTORY include/
    DESTINATION include
)

install(
    EXPORT loonTargets
    NAMESPACE loon::
    DESTINATION lib/cmake/loon
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/loonConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/loonConfigVersion.cmake"
    DESTINATION lib/cmake/loon
)

export(
    EXPORT loonTargets
    NAMESPACE loon::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/loonTargets.cmake"
)

# Testing
option(LOON_BUILD_TESTS "Build tests" OFF)
if(LOON_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Benchmarks
option(LOON_BUILD_BENCHMARKS "Build benchmarks" OFF)
if(LOON_BUILD_BENCHMARKS)
    add_subdirectory(bench)
endif()   