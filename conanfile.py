from conan import ConanFile
from conan.tools.files import copy
from conan.tools.cmake import cmake_layout, CMake
import os

class LoonLibConan(ConanFile):
    name = "loon"
    version = "0.1.0"
    description = "High-performance, header-only C++ data structures for low-latency applications"
    license = "MIT"
    author = "Jorge Suarez-Rivaya"
    url = "https://github.com/jsrivaya/loon"
    homepage = "https://github.com/jsrivaya/loon"
    topics = ("cpp", "header-only", "data-structures", "lru-cache", "performance", "low-latency", "hft")

    package_type = "header-library"
    exports_sources = "include/*", "test/*", "CMakeLists.txt", "LICENSE"
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeToolchain", "CMakeDeps"
    no_copy_source = True

    options = {"with_tests": [True, False]}
    default_options = {"with_tests": True}

    def build_requirements(self):
        if self.options.with_tests:
            self.test_requires("gtest/1.14.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={"LOON_BUILD_TESTS": "ON" if self.options.with_tests else "OFF"})
        cmake.build()
        if self.options.with_tests:
            cmake.test()

    def package(self):
        copy(self, "LICENSE",
             src=self.source_folder,
             dst=os.path.join(self.package_folder, "licenses"))
        copy(self, "*.hpp",
             src=os.path.join(self.source_folder, "include"),
             dst=os.path.join(self.package_folder, "include"))

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []   