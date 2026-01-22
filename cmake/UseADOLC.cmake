find_package(PkgConfig REQUIRED)

pkg_check_modules(ADOLC QUIET IMPORTED_TARGET GLOBAL adolc)

if (ADOLC_FOUND)
  message (STATUS "ADOL-C found: ${ADOLC_PREFIX}")
  add_library(ADOLC::ADOLC INTERFACE IMPORTED GLOBAL)
  target_link_libraries(ADOLC::ADOLC INTERFACE PkgConfig::ADOLC)
else()
  message (STATUS "ADOL-C not found! Not building with automatic differentiation.")
endif()