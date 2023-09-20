cmake_minimum_required (VERSION 3.4)
     
project(examples)
 

set(SOURCE_FILES "fuselage_wing_configuration.cpp")


add_executable (fuselage_wing_model ${SOURCE_FILES})
 
# Connecting the library, specify where to get the header files
include_directories("..")

target_link_libraries(fuselage_wing_model PUBLIC geoml)