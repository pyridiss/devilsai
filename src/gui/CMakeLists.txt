add_library(gui button.cpp style.cpp textWidget.cpp widget.cpp)

set_target_properties(gui PROPERTIES COMPILE_FLAGS "-std=c++11")
