from conan import ConanFile
from conan.tools.files import copy
from conan.tools.cmake import cmake_layout, CMake

class LooneyLibConan(ConanFile):
    name = "looneytools"
    version = "0.1.0"
    package_type = "header-library"  # Important for header-only
    exports_sources = "include/*", "test/*"
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeToolchain", "CMakeDeps"
    no_copy_source = True  # Avoid copying sources unnecessarily

    def layout(self):
        cmake_layout(self)

    def build(self):
        # Optional: build and run tests during packaging
        cmake = CMake(self)
        cmake.configure(build_script_folder="test")
        cmake.build()
        cmake.test()

    def package(self):
        # Copy headers to package folder
        copy(self, "*.hpp", src="include", dst="include")
        copy(self, "*.h", src="include", dst="include")

    def package_info(self):
        # No libraries or binaries to link
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
        # Headers are in include/mylib, so consumers can do #include <mylib/core.hpp>   