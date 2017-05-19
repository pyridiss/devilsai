add_library(tools filesystem.cpp debug.cpp textManager.cpp timeManager.cpp signals.cpp)

set_target_properties(tools PROPERTIES COMPILE_FLAGS "-std=c++11")
