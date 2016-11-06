project(robot C)
cmake_minimum_required(VERSION 2.8)
add_definitions(-g -Wall -Werror)

option(WITH_COVERAGE "Enable profiling and coverage report analysis" OFF)

if (WITH_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -ftest-coverage -fprofile-arcs")
    set(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} -lgcov -ftest-coverage -fprofile-arcs"
        CACHE INTERNAL "" FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -lgcov -ftest-coverage -fprofile-arcs"
        CACHE INTERNAL "" FORCE)
endif (WITH_COVERAGE)

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
