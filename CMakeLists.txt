cmake_minimum_required(VERSION 3.5)
project(directx_pg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT DEFINED ENV{DXSDK_INCLUDE_PATH})
    message(FATAL_ERROR "Error: no env DXSDK_INCLUDE_PATH variable set!")
elseif (NOT DEFINED ENV{DXSDK_LIB_PATH})
    message(FATAL_ERROR "Error: no env DXSDK_LIB_PATH variable set!")
elseif (NOT DEFINED ENV{DXSDK_UTILITIES_PATH})
    message(FATAL_ERROR "Error: no env DXSDK_UTILITIES_PATH variable set!")
endif()

set(DXSDK_INCLUDE_PATH $ENV{DXSDK_INCLUDE_PATH})
set(DXSDK_LIB_PATH $ENV{DXSDK_LIB_PATH})
set(DXSDK_UTILITIES_PATH $ENV{DXSDK_UTILITIES_PATH})

add_executable(app
    WIN32
        main.cpp
)

target_include_directories(app
    PRIVATE
        "${DXSDK_INCLUDE_PATH}"
)
target_link_libraries(app
    debug
        "${DXSDK_LIB_PATH}/d3dx11d.lib"

    optimized
        "${DXSDK_LIB_PATH}/d3dx11.lib"

    general
        "${DXSDK_LIB_PATH}/d3d11.lib"
        "${DXSDK_LIB_PATH}/d3dcompiler.lib"
        "${DXSDK_LIB_PATH}/dxerr.lib"
        "${DXSDK_LIB_PATH}/dxgi.lib"
        "${DXSDK_LIB_PATH}/dxguid.lib"
)

if (CMAKE_CXX_COMPILE_ID MATCHES "MSVC")
    target_compile_options(app PRIVATE /W4)
endif()
