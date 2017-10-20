cmake_minimum_required(VERSION 2.8)
project(devilsai)

set ( CMAKE_MODULE_PATH
     ${CMAKE_SOURCE_DIR}/cmake/modules
     ${CMAKE_MODULE_PATH}
)

# subdirectories to build
add_subdirectory(data)
add_subdirectory(src)


if (UNIX)
  install( PROGRAMS devilsai.desktop  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
  install( PROGRAMS data/devilsai.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/96x96/apps )
endif (UNIX)
