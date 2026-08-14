cmake_minimum_required(VERSION 3.22.1)
project(naga_sudoku_c C)

set(CMAKE_C_STANDARD 99)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/raylib/src
)

if (ANDROID)
    add_definitions(-DPLATFORM_ANDROID -DGRAPHICS_API_OPENGL_ES2)
    
    set(RAYLIB_SRC
        external/raylib/src/rcore.c
        external/raylib/src/rshapes.c
        external/raylib/src/rtextures.c
        external/raylib/src/rtext.c
        external/raylib/src/rmodels.c
        external/raylib/src/raudio.c
    )
    
    add_library(naga_sudoku SHARED
        src/main.c
        src/game.c
        src/ui_cyberpunk.c
        src/audio_synth.c
        src/sudoku_core.c
        src/sudoku_generator.c
        src/storage.c
        ${RAYLIB_SRC}
    )
    
    target_link_libraries(naga_sudoku
        android
        log
        OpenSLES
        GLESv2
        EGL
        m
    )
else()
    add_executable(naga_sudoku
        src/main.c
        src/game.c
        src/ui_cyberpunk.c
        src/audio_synth.c
        src/sudoku_core.c
        src/sudoku_generator.c
        src/storage.c
    )
    
    target_link_libraries(naga_sudoku
        ${CMAKE_CURRENT_SOURCE_DIR}/external/raylib/src/libraylib.a
        m
        pthread
        dl
        rt
        X11
    )
endif()
