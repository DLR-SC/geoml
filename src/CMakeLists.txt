#
# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2018 German Aerospace Center (DLR)
#
# Geoml Library

include(geoml-macros)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/geoml_config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/geoml_config.h"
)

# add all subdirectories to include path
set(GEOML_INCLUDES ".")
subdirlist(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
foreach(subdir ${SUBDIRS})
  set(GEOML_INCLUDES ${GEOML_INCLUDES} ${subdir})
endforeach()

file(GLOB_RECURSE SOURCES
    "*.h"
    "*.hxx"
    "*.cpp"
    "*.cxx"
)

# group sources as on filesystem
foreach(source ${SOURCES})
    get_filename_component(sourcePath "${source}" DIRECTORY)
    file(RELATIVE_PATH sourcePath ${CMAKE_CURRENT_SOURCE_DIR} ${sourcePath})
    string(REPLACE "/" "\\" sourcePathNoBS "${sourcePath}")
    source_group("${sourcePathNoBS}" FILES "${source}")
endforeach()

# object library containing just the compiled sources
add_library(geoml ${SOURCES})
target_compile_features(geoml PRIVATE cxx_std_17)

include(GenerateExportHeader)
generate_export_header(geoml
    EXPORT_MACRO_NAME GEOML_API_EXPORT
    EXPORT_FILE_NAME geoml/exports.h
)

target_link_libraries(geoml PUBLIC ${OpenCASCADE_LIBRARIES} )

target_include_directories(geoml
    PRIVATE ${GEOML_INCLUDES}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

if(MSVC)
    target_compile_definitions(geoml PRIVATE _CRT_SECURE_NO_WARNINGS=1 /wd4355 GEOML_INTERNAL_EXPORTS)
    target_link_libraries(geoml PRIVATE ShlWapi PUBLIC Ws2_32)
endif(MSVC)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(geoml PRIVATE DEBUG)
endif()

target_include_directories(geoml PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

set_target_properties( geoml PROPERTIES VERSION ${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set_target_properties( geoml PROPERTIES SOVERSION "${PROJECT_VERSION_MINOR}")

install (TARGETS geoml
         EXPORT geoml-targets
         RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY geoml/
    DESTINATION include/geoml
    COMPONENT cpp_bindings
    FILES_MATCHING
    PATTERN "*.h"
)

install(FILES
 ${CMAKE_CURRENT_BINARY_DIR}/geoml/exports.h DESTINATION include/geoml
)

add_library(geoml_internal INTERFACE)

foreach(directory ${GEOML_INCLUDES})
  target_include_directories(geoml_internal INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${directory}>
    $<INSTALL_INTERFACE:include/geoml/internal/${directory}>
  )
endforeach()
target_include_directories(geoml_internal INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(geoml_internal INTERFACE ${OpenCASCADE_LIBRARIES})
target_compile_features(geoml_internal INTERFACE cxx_std_17)



# cmake configuration export
set(CMAKE_INSTALL_INCLUDE_DIR "include")
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/geoml")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/geoml-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)


file(GENERATE
  OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/geoml-config.cmake"
  INPUT  "${PROJECT_SOURCE_DIR}/cmake/geoml-config.cmake.in"
)

install (EXPORT geoml-targets DESTINATION ${CONFIG_INSTALL_DIR})

install (FILES
  "${CMAKE_CURRENT_BINARY_DIR}/geoml-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/geoml-config-version.cmake"
  DESTINATION ${CONFIG_INSTALL_DIR})

# --------------------- Install GEOML internal api ----------------- #

if (GEOML_INSTALL_INTERNAL_API)
    
    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/geoml_internal-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/geoml_internal-config.cmake"
        @ONLY
    )
    
    install (TARGETS geoml
             EXPORT geoml_internal-targets
             RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
             ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    install(TARGETS geoml_internal EXPORT geoml_internal-targets)
    install (EXPORT geoml_internal-targets DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/geoml_internal)
    
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/geoml_config.h
        DESTINATION include/geoml/internal COMPONENT cpp_bindings)
    
    install(DIRECTORY .
        DESTINATION include/geoml/internal
        COMPONENT cpp_bindings
        FILES_MATCHING  PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.hxx"
        PATTERN "geoml/*" EXCLUDE
    )


    install (FILES
      "${CMAKE_CURRENT_BINARY_DIR}/geoml_internal-config.cmake"
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/geoml_internal)
    
    write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/geoml_internal-config-version.cmake"
      VERSION ${PROJECT_VERSION}
      COMPATIBILITY AnyNewerVersion
    )

endif (GEOML_INSTALL_INTERNAL_API)
