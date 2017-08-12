add_library(imageManager imageManager.cpp image.cpp animation.cpp HSL.cpp)

target_link_libraries(imageManager ${SFML_LIBRARIES} ${PHYSFS_LIBRARY})
