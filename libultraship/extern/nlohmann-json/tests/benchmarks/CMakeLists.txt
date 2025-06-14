cmake_minimum_required(VERSION 3.16...3.31.5)
project(JSON_Benchmarks LANGUAGES CXX)

# set compiler flags
if((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -DNDEBUG -O3")
endif()

# configure Google Benchmarks
include(FetchContent)
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG origin/main
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(benchmark)
if(NOT benchmark_POPULATED)
    FetchContent_Populate(benchmark)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(${benchmark_SOURCE_DIR} ${benchmark_BINARY_DIR})
endif()

# download test data
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake ${CMAKE_MODULE_PATH})
include(download_test_data)

# benchmark binary
add_executable(json_benchmarks src/benchmarks.cpp)
target_compile_features(json_benchmarks PRIVATE cxx_std_11)
target_link_libraries(json_benchmarks benchmark ${CMAKE_THREAD_LIBS_INIT})
add_dependencies(json_benchmarks download_test_data)
target_include_directories(json_benchmarks PRIVATE ${CMAKE_SOURCE_DIR}/../../single_include ${CMAKE_BINARY_DIR}/include)
