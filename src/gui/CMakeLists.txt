add_library(gui button.cpp style.cpp textWidget.cpp widget.cpp window.cpp)

target_link_libraries(gui tinyxml2)

set_target_properties(gui PROPERTIES COMPILE_FLAGS "-std=c++11")
