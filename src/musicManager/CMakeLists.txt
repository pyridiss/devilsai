add_library(musicManager musicManager.cpp)

target_link_libraries(musicManager sfml-audio)

set_target_properties(musicManager PROPERTIES COMPILE_FLAGS "-std=c++11")
