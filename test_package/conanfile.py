from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMake
from conan.tools.build import can_run

class LoonTestConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if can_run(self):
            cmd = f"./{self.cpp.build.bindir}/example"
            self.run(cmd, env="conanrun")
