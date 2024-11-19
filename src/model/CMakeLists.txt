file(GLOB_RECURSE model_SRC *.cpp)

add_library(model STATIC ${model_SRC})
