from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conan.tools.files import load
import os
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
    requires = "opencascade/7.6.2@dlr-sc/stable"
    exports_sources =  "src/*", "CMakeLists.txt", "cmake/*", "docs/*", "bindings/*", "LICENSE.txt"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")
        self.options["opencascade"].shared = self.options.shared
        self.options["opencascade"].patch_GeomFill_CoonsC2Style = True

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.cache_variables["GEOML_INSTALL_INTERNAL_API"] = True
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
        if self.options.shared:
            self.cpp_info.requires = ["opencascade::opencascade"]
        # this can't be the correct way to set the include dirs of geoml_internal...
        self.cpp_info.includedirs.append(os.path.join("include", "geoml"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "curves"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "data_structures"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "geom_topo"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "surfaces"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "utility"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "naming_choosing"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "boolean_operations"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "common"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "contrib"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "exports"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "geometry"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "imports"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "logging"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "math"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "system"))
        self.cpp_info.includedirs.append(os.path.join("include", "geoml", "internal", "topology"))



