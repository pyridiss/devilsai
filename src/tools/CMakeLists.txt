add_library(tools filesystem.cpp debug.cpp math.cpp textManager.cpp timeManager.cpp shape.cpp signals.cpp vector2d.cpp)

target_link_libraries(tools ${SFML_LIBRARIES} ${TinyXML2_LIBRARIES})
