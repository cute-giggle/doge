set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB test_melo_SRC *.cpp)

add_executable(test_melo ${test_melo_SRC})

target_link_libraries(test_melo
    -Wl,--start-group
    language
    model
    utils
    -Wl,--end-group
    openvino
    spdlog
    sndfile
)
