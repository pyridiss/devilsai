add_library(tools filesystem.cpp debug.cpp math.cpp textManager.cpp timeManager.cpp shape.cpp signals.cpp vector2d.cpp)

set_target_properties(tools PROPERTIES COMPILE_FLAGS "-std=c++11")
