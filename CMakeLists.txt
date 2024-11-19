cmake_minimum_required(VERSION 3.16.0)

project(doge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(thirdparty_dir ${PROJECT_SOURCE_DIR}/thirdparty)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${thirdparty_dir}/spdlog/include
    ${thirdparty_dir}/cppjieba/include
    ${thirdparty_dir}/libsndfile/include
    ${thirdparty_dir}/openvino/runtime/include
)

link_directories(
    ${thirdparty_dir}/spdlog/lib
    ${thirdparty_dir}/libsndfile/lib
    ${thirdparty_dir}/openvino/runtime/lib/intel64
)

add_subdirectory(src)
add_subdirectory(test)
