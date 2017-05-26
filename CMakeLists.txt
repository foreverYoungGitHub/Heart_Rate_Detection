cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME "Heart_Rate_Detection")
project("${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV)

file(GLOB inc "./*.h" "./color_magnify/*.h")
file(GLOB src "./*.cpp" "./color_magnify/*.cpp")

add_executable(${PROJECT_NAME} ${inc} ${src})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} )