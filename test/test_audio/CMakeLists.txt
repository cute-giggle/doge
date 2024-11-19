set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(GLOB test_audio_SRC *.cpp)

add_executable(test_audio ${test_audio_SRC})

target_link_libraries(test_audio whisper sndfile)
