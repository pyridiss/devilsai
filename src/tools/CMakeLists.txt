add_library(tools aStar.cpp filesystem.cpp debug.cpp math.cpp timeManager.cpp shape.cpp signals.cpp vector2d.cpp)

target_link_libraries(tools ${SFML_LIBRARIES} ${TinyXML2_LIBRARIES})
