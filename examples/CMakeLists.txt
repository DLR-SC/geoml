cmake_minimum_required (VERSION 3.4)
     
project(examples)
 
#set(SOURCE_FILES "test_main.cpp")
set(SOURCE_FILES_2 "test_main_2.cpp")

#add_executable (geoModel ${SOURCE_FILES})
add_executable (geomlModel_2 ${SOURCE_FILES_2})
 
# Connecting the library, specify where to get the header files
include_directories("..")

#target_link_libraries(geoModel PUBLIC geolib)
target_link_libraries(geomlModel_2 PUBLIC geoml)