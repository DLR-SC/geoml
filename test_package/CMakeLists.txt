#
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
#

cmake_minimum_required(VERSION 3.15)

project(PackageTest CXX)

find_package(geoml CONFIG REQUIRED)

add_executable(example example.cpp)
target_link_libraries(example geoml::geoml)
