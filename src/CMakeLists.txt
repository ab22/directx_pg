cmake_minimum_required(VERSION 3.5)
project(directx_engine LANGUAGES CXX)

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
    message(FATAL_ERROR "> Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

target_link_libraries(app
    PRIVATE
    general
        "d3d11.lib"
        "d3dcompiler.lib"
        "dxgi.lib"
        "dxguid.lib"
        "fmt-header-only"
)
