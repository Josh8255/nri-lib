cmake_minimum_required(VERSION 3.18.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(nri VERSION 0.1.0)

file(GLOB_RECURSE nri_SRC
     "src/*.h"
     "src/*.cpp"
)

add_executable(nri ${nri_SRC})