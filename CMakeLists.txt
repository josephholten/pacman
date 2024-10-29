cmake_minimum_required(VERSION 3.20)
project(pacman VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SAN -fsanitize=address)
set(FLAGS -g -O0 -Wall -Wextra -Wpedantic -Wno-missing-field-initializers)

include_directories(.)
add_subdirectory(raylib)

add_compile_definitions(ASSETS_PATH="${PROJECT_SOURCE_DIR}/assets")

add_executable(main main.cpp)
target_link_libraries(main raylib)
target_compile_options(main PRIVATE ${FLAGS} ${SAN})
target_link_options(main PRIVATE ${SAN})
