add_library(imageManager assets.cpp imageManager.cpp image.cpp HSL.cpp)

target_link_libraries(imageManager ${SFML_LIBRARIES} ${PHYSFS_LIBRARY})
