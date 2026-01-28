cmake_minimum_required(VERSION 3.20)
project(loon LANGUAGES CXX)

# Interface target for the header-only library
add_library(loon INTERFACE)
target_include_directories(loon
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Optional: generate and install config package
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/loonConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
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

export(
    EXPORT loonTargets
    NAMESPACE loon::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/loonTargets.cmake"
)   