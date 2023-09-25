
add_executable(geoml-apitest
    src/testSurfaceModeling.cpp
    src/testCurveModeling.cpp
    src/main.cpp
)
target_link_libraries(geoml-apitest PUBLIC gtest geoml)

ADD_TEST(
  NAME apitests
  COMMAND geoml-apitest  --gtest_output=xml
)
