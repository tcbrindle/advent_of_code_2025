
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
    "d0edc3af-4c50-42ea-a356-e2862fe7a444")

cmake_minimum_required(VERSION 4.2)
project(aoc2025 CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_MODULE_STD ON)

include(FetchContent)

FetchContent_Declare(
    flux
    GIT_REPOSITORY https://github.com/tcbrindle/flux.git
    GIT_TAG devel
)
set(FLUX_BUILD_MODULE On)

FetchContent_Declare(
    ctre
    GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
    GIT_TAG main
)
set(CTRE_MODULE On)

FetchContent_Declare(
    unordered_dense
    GIT_REPOSITORY https://github.com/martinus/unordered_dense.git
    GIT_TAG v4.8.1
)
set(ANKERL_ENABLE_MODULES On)

FetchContent_MakeAvailable(flux ctre unordered_dense)

add_library(aoc STATIC)
add_library(aoc::aoc ALIAS aoc)
target_sources(
    aoc PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS .
    FILES aoc.cpp
)
target_link_libraries(aoc PUBLIC ctre::ctre flux::module unordered_dense::unordered_dense_module)

function(ADD_DAY DATE)
    add_executable(${DATE} ${DATE}/main.cpp)
    target_link_libraries(${DATE} PRIVATE aoc::aoc)
endfunction()

add_day(dec01)
add_day(dec02)
add_day(dec03)
add_day(dec04)
add_day(dec05)
add_day(dec06)
add_day(dec07)
add_day(dec08)
add_day(dec09)
add_day(dec11)
add_day(dec12)
