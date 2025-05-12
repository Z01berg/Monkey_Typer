cmake_minimum_required(VERSION 3.14)
project(MonkeyTyper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Global Build Configuration ---
set(BUILD_SHARED_LIBS FALSE CACHE BOOL "Build shared libraries" FORCE)

# --- Dependencies via FetchContent ---
include(FetchContent)

FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
        GIT_SHALLOW TRUE
)
set(SFML_BUILD_AUDIO TRUE CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK FALSE CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(sfml)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 9.1.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(fmt)

# --- Executable Target Definition ---
add_executable(monkey_typer
        src/Button.cpp
        src/TextAnimation.cpp
        src/Word.cpp
        src/LinkedWord.cpp
        src/MonkeyTyperGame.cpp
        src/main.cpp
        src/FontManager.cpp
        src/WordDatabaseManager.cpp
        src/SettingsManager.cpp
        src/HighScoreManager.cpp
        src/UIManager.cpp
)

target_include_directories(monkey_typer PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${sfml_SOURCE_DIR}/include"
        "${fmt_SOURCE_DIR}/include"
)

# --- Linking Stage ---
target_compile_definitions(monkey_typer PRIVATE SFML_STATIC)

target_link_libraries(monkey_typer PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        fmt::fmt
)

# --- Platform-specific for SFML ---
if(WIN32)
    target_link_libraries(monkey_typer PRIVATE
            opengl32
            winmm
            gdi32
            # For SFML audio, if OpenAL is not built-in to sfml-audio-s by default:
            # openal32
            # And if FreeType is not built-in to sfml-graphics-s:
            # freetype
    )
elseif(UNIX AND NOT APPLE) # Linux example
    target_link_libraries(monkey_typer PRIVATE
            pthread
            # X11 Xrandr udev # Usually found by SFML's Find modules for system features
            # GL openal freetype # If not handled by SFML's static lib itself
    )
endif()

# --- Asset Management ---
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    endif()
endif()

file(COPY "${PROJECT_SOURCE_DIR}/dictionary" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/dictionary")
file(COPY "${PROJECT_SOURCE_DIR}/fonts" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fonts")
file(COPY "${PROJECT_SOURCE_DIR}/util" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/util")

# Example if you had a file directly in project root to copy (not currently needed for your structure)
# file(COPY "${PROJECT_SOURCE_DIR}/PixelTCG.ttf" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")