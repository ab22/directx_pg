cmake_minimum_required(VERSION 3.12)
project(directx_pg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/W4)
	add_compile_definitions(-DWIN32_LEAN_AND_MEAN)
else()
    message(FATAL_ERROR "> Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

add_subdirectory("src")
# fmt header only.
add_subdirectory("vendor/fmt" EXCLUDE_FROM_ALL)
