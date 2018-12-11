cmake_minimum_required(VERSION 3.5)
project(directx_pg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(app
    WIN32
        main.cpp
        game_timer.cpp
        d3dapp.cpp
        demo_app.cpp
        logger.cpp
        errors.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(app PRIVATE /W4)
    target_compile_definitions(app PRIVATE -DWIN32_LEAN_AND_MEAN)
else()
    message(FATAL_ERROR "> Unsupported ompiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# fmt header only.
add_subdirectory("vendor/fmt" EXCLUDE_FROM_ALL)

target_link_libraries(app
    PRIVATE
    general
        "d3d11.lib"
        "d3dcompiler.lib"
        "dxgi.lib"
        "dxguid.lib"
        "fmt-header-only"
)
