cmake_minimum_required(VERSION 2.8)
project(Heart_Rate_Detection)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV)

file(GLOB inc "./*.h")
file(GLOB src "./*.cpp")

add_executable(Heart_Rate_Detection ${inc} ${src})