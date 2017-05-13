add_library(tools button.cpp style.cpp debug.cpp)

set_target_properties(tools PROPERTIES COMPILE_FLAGS "-std=c++11")
