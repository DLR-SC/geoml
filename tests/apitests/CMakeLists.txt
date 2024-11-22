
add_executable(geoml-apitest
    src/testSurfaces.cpp
    src/testCurves.cpp
    src/testGeomTopoConversions.cpp
    src/testDataStructures.cpp
    src/testNamingChoosing.cpp
    src/testUtilities.cpp
    src/main.cpp
)
target_link_libraries(geoml-apitest PUBLIC gtest geoml)

ADD_TEST(
  NAME apitests
  COMMAND geoml-apitest  --gtest_output=xml
)
