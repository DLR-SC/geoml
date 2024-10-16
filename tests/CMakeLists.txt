#
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
#


# Set a default build type if none was specified
if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

cmake_minimum_required(VERSION 3.0)
project (geoml-tests)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (NOT TARGET geoml_internal)
    find_package(geoml_internal CONFIG REQUIRED)
endif()

set(BUILD_SHARED_LIBS OFF)
if (NOT TARGET gtest)
    include(AddGoogleTest)
endif()

add_subdirectory(common)
add_subdirectory(unittests)
add_subdirectory(apitests)
