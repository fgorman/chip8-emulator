cmake_minimum_required(VERSION 3.0.0)
project(chip8 VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(
    chip8
    src/main.c
    src/chip8.c
    src/io.c
)

target_link_libraries(
    chip8
    ${SDL2_LIBRARIES}
)

target_include_directories(
    chip8
    PUBLIC
    "${PROJECT_SOURCE_DIR}/inc"
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
