cmake_minimum_required(VERSION 3.12)
project(directx_engine LANGUAGES CXX)

add_executable(app
    WIN32
        main.cpp
        game_timer.cpp
        d3dapp.cpp
        demo_app.cpp
        logger.cpp
        errors.cpp
        debug_utils.cpp
        com_ptr.h
        d3dapp.h
        debug_utils.h
        demo_app.h
        errors.h
        game_timer.h
        logger.h
        win_utils.h
        vertex.h

        shaders/vertex1_fx.fx
        shaders/vertex1_ps.hlsl
        shaders/vertex1_vs.hlsl
)

target_include_directories(app
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/vendor/expected"
)

target_link_libraries(app
    PRIVATE
    general
        "d3d11.lib"
        "d3dcompiler.lib"
        "dxgi.lib"
        "dxguid.lib"
        "fmt-header-only"
)

set_source_files_properties(
    shaders/vertex1_vs.hlsl
    PROPERTIES
        VS_SHADER_TYPE
            Vertex
        VS_SHADER_MODEL
            5.0
        VS_SHADER_ENTRYPOINT
            vertex_shader
        VS_SHADER_ENABLE_DEBUG
            $<$<CONFIG:DEBUG>:TRUE>
        VS_SHADER_DISABLE_OPTIMIZATIONS
            $<$<CONFIG:DEBUG>:TRUE>
)

set_source_files_properties(
    shaders/vertex1_ps.hlsl
    PROPERTIES
        VS_SHADER_TYPE
            Pixel
        VS_SHADER_MODEL
            5.0
        VS_SHADER_ENTRYPOINT
            pixel_shader
        VS_SHADER_ENABLE_DEBUG
            $<$<CONFIG:DEBUG>:TRUE>
        VS_SHADER_DISABLE_OPTIMIZATIONS
            $<$<CONFIG:DEBUG>:TRUE>
)
