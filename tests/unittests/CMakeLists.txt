file (GLOB test_SRCS *.cpp *.cxx *.cc *.C *.c *.h *.hpp)

set (test_BIN "geoml-unittests")


add_executable(${test_BIN} ${test_SRCS} )
target_link_libraries(${test_BIN} PUBLIC gtest geoml geoml_internal geomlCommonTestUtils)

ADD_CUSTOM_TARGET(check_geoml ${test_BIN} --gtest_output=xml DEPENDS ${test_BIN} COMMENT "Executing unit tests..." VERBATIM SOURCES ${test_SRCS})

ADD_TEST(
  NAME unittests
  COMMAND ${test_BIN} --gtest_output=xml
)

add_custom_command(
        TARGET ${test_BIN} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/TestData
                ${CMAKE_CURRENT_BINARY_DIR}/TestData)

#create directory to avoid crash in unit tests
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/TestData/export)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/TestData/analysis)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/TestData/bugs/505)
