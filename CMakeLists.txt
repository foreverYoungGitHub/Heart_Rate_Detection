cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME "Heart_Rate_Detection")
project("${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Caffe)
include_directories(${Caffe_INCLUDE_DIRS})

add_subdirectory(color_magnify)
add_subdirectory(MTCNN)
add_subdirectory(skcf)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${Caffe_LIBRARIES} color_magnify MTCNN skcf)