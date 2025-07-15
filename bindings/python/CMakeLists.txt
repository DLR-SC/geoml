#
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
#

# To build this project, call pip install .

cmake_minimum_required(VERSION 3.4)
project(geoml-python VERSION 0.1)

set(CMAKE_INSTALL_RPATH $ORIGIN)

if (POLICY CMP0086)
  cmake_policy(SET CMP0086 OLD)
endif (POLICY CMP0086)

if (POLICY CMP0078)
  cmake_policy(SET CMP0078 OLD)
endif (POLICY CMP0078)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# build geoml first
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../ ${CMAKE_CURRENT_BINARY_DIR}/native EXCLUDE_FROM_ALL)

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
message(STATUS "Python3 interpreter:" ${Python3_EXECUTABLE})
message(STATUS "Python include directory: ${Python3_INCLUDE_DIR}")
message(STATUS "Python library release: ${Python3_LIBRARY_RELEASE}")


find_package(SWIG 3.0.11 REQUIRED)
find_package(PythonOCC REQUIRED)
find_package(Doxygen 1.8.0)

if(NOT DOXYGEN_FOUND)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doc.i
        COMMAND ${CMAKE_COMMAND}
        ARGS -E touch ${CMAKE_CURRENT_BINARY_DIR}/doc.i
    )
else()
    # we build the swig documentation as follows
    # 1) Run doxygen to create an xml output
    # 2) Run doxy2swig to create doc.i from the xml output
    # 3) Include doc.i in the swig wrapper files

    # create the Doxyfile
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    # create the xml output with doxygen
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doc/xml/index.xml
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        COMMAND ${DOXYGEN_EXECUTABLE}
        ARGS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    )

    # create the doc.i with doxy2swig
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doc.i
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doc/xml/index.xml
        COMMAND ${PYTHON_EXECUTABLE}
        ARGS ${PROJECT_SOURCE_DIR}/thirdparty/doxy2swig/doxy2swig.py ${CMAKE_CURRENT_BINARY_DIR}/doc/xml/index.xml ${CMAKE_CURRENT_BINARY_DIR}/doc.i
    )
endif()


include(${SWIG_USE_FILE})

# we use the same definitions as pythonocc for their build to hide some of their warnings
option(SWIG_HIDE_WARNINGS "Check this option if you want a less verbose swig output." ON)
set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -py3)
if(SWIG_HIDE_WARNINGS)
  message(STATUS "Disabled SWIG warnings")
  set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -w302,401,402,412,314,509,512,504,325,503,520,350,351,383,389,394,395,404)
endif()


include_directories(${PYTHON_INCLUDE_PATH})

include_directories(${PythonOCC_INCLUDE_DIRS})

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

include_directories($<TARGET_PROPERTY:geoml,INTERFACE_INCLUDE_DIRECTORIES>)

set_source_files_properties(pygeoml.i PROPERTIES CPLUSPLUS ON)
set(SWIG_MODULE_pygeoml_EXTRA_DEPS common.i doc.i)

swig_add_library(pygeoml LANGUAGE python SOURCES pygeoml.i TYPE MODULE)
swig_link_libraries(pygeoml geoml Python3::Module)

install(TARGETS _pygeoml LIBRARY DESTINATION geoml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pygeoml.py
        DESTINATION geoml
)


