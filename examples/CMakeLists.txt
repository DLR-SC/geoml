#cmake_minimum_required (VERSION 3.4)
     
#project(examples)
 

#set(SOURCE_FILES "fuselage_wing_configuration.cpp")


#add_executable (fuselage_wing_model ${SOURCE_FILES})
 
# Connecting the library, specify where to get the header files
#include_directories("..")

#target_link_libraries(fuselage_wing_model PUBLIC geoml)

cmake_minimum_required (VERSION 3.13)
     
project(examples)
 
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(geoml REQUIRED)

add_executable (fuselage_wing_model fuselage_wing_configuration.cpp)

target_link_libraries(fuselage_wing_model PUBLIC geoml::geoml)
target_compile_features(fuselage_wing_model PRIVATE cxx_std_17)