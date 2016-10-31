add_library(imageManager imageManager.cpp image.cpp)

set_target_properties(imageManager PROPERTIES COMPILE_FLAGS "-std=c++11")
