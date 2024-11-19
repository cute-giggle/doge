set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB test_whisper_SRC *.cpp)

add_executable(test_whisper ${test_whisper_SRC})

target_link_libraries(test_whisper
    sndfile
    whisper
)
