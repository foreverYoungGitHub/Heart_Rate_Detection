cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

get_directory_property(HAS_PARENT PARENT_DIRECTORY)
set(SKCF_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src")
if (HAS_PARENT)
    set(SKCF_INCLUDE ${SKCF_INCLUDE} PARENT_SCOPE)
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(SKCF_LIB_SRC ktrackers.h ktrackers.cpp gradient.h gradient.cpp)

add_library(skcf STATIC ${SKCF_LIB_SRC})

target_link_libraries(skcf ${OpenCV_LIBS})
