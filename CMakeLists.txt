

cmake_minimum_required(VERSION 4.0)
project(Berrie VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        12.1.0)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        magic_enum
        GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
        GIT_TAG        v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

add_subdirectory(src)
