cmake_minimum_required(VERSION 3.23)
project(202302_Spectrum_MOT)

set(CMAKE_CXX_STANDARD 14)

#include_directories("D:/Applications/Python-3.7.6/include")
#link_directories("D:/Applications/Python-3.7.6/libs")

#include_directories(D:/Applications/Python-3.7.6/include/)
#link_libraries(D:/Applications/Python-3.7.6/include/libs/python37.lib)

# reference: https://stackoverflow.com/a/72162717
find_package(Python REQUIRED Development)

add_executable(202302_Spectrum_MOT main.cpp utils.h utils_input.cpp utils_input.h)
target_link_libraries(202302_Spectrum_MOT PRIVATE Python::Python)
