cmake_minimum_required(VERSION 3.20)
project(PackageTest CXX)

find_package(loon REQUIRED)

add_executable(example src/example.cpp)
target_link_libraries(example PRIVATE loon::loon)
target_compile_features(example PRIVATE cxx_std_23)
