add_library(textManager plainText.cpp richText.cpp)

target_link_libraries(textManager ${SFML_LIBRARIES})
