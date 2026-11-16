cmake_minimum_required(VERSION 3.25)
project(chess_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(CHESS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)
option(CHESS_SANITIZERS "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)
option(CHESS_NATIVE "Tune for the build machine (-march=native)" OFF)

# ---------------------------------------------------------------------------
# chess_options: link every target against this to get warnings / sanitizers.
#   target_link_libraries(<target> PRIVATE chess_options)
# ---------------------------------------------------------------------------
add_library(chess_options INTERFACE)

target_compile_options(chess_options INTERFACE
    -Wall -Wextra -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion
    -Wold-style-cast -Wcast-align -Wnull-dereference
    -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough
    -Wnon-virtual-dtor -Woverloaded-virtual
    $<$<BOOL:${CHESS_WARNINGS_AS_ERRORS}>:-Werror>
)

if(CHESS_SANITIZERS)
    target_compile_options(chess_options INTERFACE
        -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer)
    target_link_options(chess_options INTERFACE -fsanitize=address,undefined)
    target_compile_definitions(chess_options INTERFACE _GLIBCXX_ASSERTIONS)
endif()

if(CHESS_NATIVE)
    target_compile_options(chess_options INTERFACE -march=native)
endif()

# Keep frame pointers in profiling builds so `perf record -g` gives usable stacks.
target_compile_options(chess_options INTERFACE
    $<$<CONFIG:RelWithDebInfo>:-fno-omit-frame-pointer>)

# ---------------------------------------------------------------------------
# Legacy sources: written before chess_options existed. They still get every
# warning, but not -Werror (source-file options come after the target's, so
# -Wno-error wins). Remove files from this list as they are cleaned up, and
# delete the list once it is empty.
# ---------------------------------------------------------------------------
set(CHESS_LEGACY_SOURCES
    source/game.cc
    source/playerengineminimax.cc
    source/testsuite.cc
    source/interface.cc
    source/main.cc
)
set_source_files_properties(${CHESS_LEGACY_SOURCES}
    PROPERTIES COMPILE_OPTIONS -Wno-error)

# ---------------------------------------------------------------------------
# SDL2 + SDL2_image (apt: libsdl2-dev libsdl2-image-dev).
# Wrapped in a SYSTEM target so -Werror ignores warnings in SDL's headers.
# ---------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2 SDL2_image)
set_target_properties(PkgConfig::SDL2 PROPERTIES SYSTEM ON)

# ---------------------------------------------------------------------------
# Targets
# ---------------------------------------------------------------------------

# Game logic: board, move generation, engines. No SDL, so it can be tested
# and benchmarked headless.
add_library(chess_core STATIC
    source/game.cc
    source/playerengineminimax.cc
    source/testsuite.cc
)
target_link_libraries(chess_core PRIVATE chess_options)
target_include_directories(chess_core PUBLIC include)
# Temporary: consumers see the legacy headers as system headers, so their
# warnings don't break -Werror in new code (the tests). Remove once the
# headers are warning-clean.
set_target_properties(chess_core PROPERTIES SYSTEM ON)

# SDL front end. The piece images are loaded from "../pieces/" relative to the
# working directory, so the binary goes in bin/ with a copy of pieces/ next to
# it: run it as `cd build/<preset>/bin && ./chess_game`.
add_executable(chess_game
    source/main.cc
    source/interface.cc
)
target_link_libraries(chess_game PRIVATE chess_options chess_core PkgConfig::SDL2)
set_target_properties(chess_game PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_custom_command(TARGET chess_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/pieces ${CMAKE_BINARY_DIR}/pieces)

enable_testing()

# ---------------------------------------------------------------------------
# Tests — GoogleTest, fetched at configure time.
# SYSTEM marks its headers as system headers, so our strict warnings
# (-Wold-style-cast etc. + -Werror) don't fire on code we don't own.
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    SYSTEM
)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(game_test tests/game_test.cc)
target_link_libraries(game_test PRIVATE chess_options chess_core GTest::gtest_main)
gtest_discover_tests(game_test PROPERTIES TIMEOUT 60)

add_executable(perft_test tests/perft_test.cc)
target_link_libraries(perft_test PRIVATE chess_options chess_core GTest::gtest_main)
gtest_discover_tests(perft_test PROPERTIES TIMEOUT 300)
