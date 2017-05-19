cmake_minimum_required(VERSION 2.8)
project(devilsai)

set ( CMAKE_MODULE_PATH
     ${CMAKE_SOURCE_DIR}/cmake/modules
     ${CMAKE_MODULE_PATH}
)

find_package(SFML 2 COMPONENTS system window graphics REQUIRED)
find_package(PhysFS)
find_package(Lua52)
find_package(TinyXML2)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

# subdirectories to build
add_subdirectory(data)
add_subdirectory(src)


# files to install in the devilsai project root directory
install( PROGRAMS devilsai.desktop  DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
install( PROGRAMS data/devilsai.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/96x96/apps )
