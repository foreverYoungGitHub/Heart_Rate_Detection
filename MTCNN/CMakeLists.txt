cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)

get_directory_property(HAS_PARENT PARENT_DIRECTORY)
set(MTCNN_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}")
if (HAS_PARENT)
    set(Color_Magnify_INCLUDE ${SKCF_INCLUDE} PARENT_SCOPE)
endif()

find_package(Caffe)
include_directories(${Caffe_INCLUDE_DIRS})
#message(status ${Caffe_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(MTCNN_LIB_SRC MTCNN.cpp MTCNN.h)

add_library(MTCNN STATIC ${MTCNN_LIB_SRC})

target_link_libraries(MTCNN ${OpenCV_LIBS} )
target_link_libraries(MTCNN ${Caffe_LIBRARIES})