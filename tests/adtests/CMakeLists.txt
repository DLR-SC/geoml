
add_executable(geoml-adtest
    src/adTestCurves.cpp
    src/adTestGordonSurfaces.cpp
    src/main.cpp
)
target_link_libraries(geoml-adtest PUBLIC gtest geoml)

ADD_TEST(
  NAME adtests
  COMMAND geoml-adtest  --gtest_output=xml
)
