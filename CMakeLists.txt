cmake_minimum_required(VERSION 3.5)
project(directx_pg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory("src")
# fmt header only.
add_subdirectory("vendor/fmt" EXCLUDE_FROM_ALL)
