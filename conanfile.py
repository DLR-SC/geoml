from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conan.tools.files import load
import re

def get_version(conanfile = None):
    try:
        content = load(conanfile, "CMakeLists.txt")
        version = re.search("project \(geoml VERSION (.*)\)", content).group(1)
        return version.strip()
    except Exception as e:
        return None

class geomlConan(ConanFile):
    name = "geoml"
    version = get_version()
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of geo here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    requires = "opencascade/7.6.2"
    exports_sources =  "src/*", "CMakeLists.txt", "cmake/*", "docs/*", "bindings/*", "LICENSE.txt"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")
        self.options["opencascade"].shared = self.options.shared

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        cmake = CMakeDeps(self)
        cmake.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["geoml"]


