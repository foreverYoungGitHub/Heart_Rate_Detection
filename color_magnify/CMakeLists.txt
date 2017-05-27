cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)


get_directory_property(HAS_PARENT PARENT_DIRECTORY)
set(Color_Magnify_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}")
if (HAS_PARENT)
    set(Color_Magnify_INCLUDE ${Color_Magnify_INCLUDE} PARENT_SCOPE)
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


set(Color_Magnify_LIB_SRC color_magnify.cpp color_magnify.h)

add_library(color_magnify STATIC ${Color_Magnify_LIB_SRC})

target_link_libraries(color_magnify ${OpenCV_LIBS} )





