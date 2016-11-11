add_library(imageManager imageManager.cpp image.cpp animation.cpp)

set_target_properties(imageManager PROPERTIES COMPILE_FLAGS "-std=c++11")
