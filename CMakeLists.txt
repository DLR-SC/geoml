# Set a default build type if none was specified
if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

cmake_minimum_required (VERSION 3.15.0)

project (geoml VERSION 3.3.0)

# enable C++17 support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)


if(NOT DEFINED CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR "bin")
endif(NOT DEFINED CMAKE_INSTALL_BINDIR)

# these settings are required in order to create fully relocatable
# libraries on osx
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# convert path to absolute (required for some scripts)
if (NOT IS_ABSOLUTE ${CMAKE_INSTALL_PREFIX})
   set (CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_PREFIX})
endif()

option(GEOML_INSTALL_INTERNAL_API "Install geoml's CPP bindings" OFF)


set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

include(UseOpenCASCADE)


# enable parallel builds in Visual Studio
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

# style checks
add_custom_target(checkstyle)
add_custom_target(checkstylexml)

add_subdirectory(src)

#create gtests, override gtest standard setting
option(GEOML_BUILD_TESTS "Build geoml Testsuite" OFF)

if(GEOML_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif(GEOML_BUILD_TESTS)

add_subdirectory(docs)

option(GEOML_EXAMPLES "Build geoml examples" OFF)

if (GEOML_EXAMPLES)
    add_subdirectory(examples)
endif()

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Martin Siggel") #required for debian/ubuntu
set(CPACK_PACKAGE_VENDOR "www.dlr.de/sc")
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE.txt)

# set generators
if(CPACK_GENERATOR)    
    #already set
elseif(APPLE)
    set(CPACK_GENERATOR DragNDrop)
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
else()
    set(CPACK_GENERATOR TGZ)
endif()


include(CPack)
cpack_add_component(headers DISPLAY_NAME "Headers")
cpack_add_component(cpp_bindings DISPLAY_NAME "Internal C++ Bindings")
cpack_add_component(interfaces DISPLAY_NAME "Interfaces/Bindings")
cpack_add_component(docu DISPLAY_NAME "Documentation")
