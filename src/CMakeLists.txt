project(devilsai)

set(VERSION "master")
string(TIMESTAMP TIME %Y-%m-%d)

configure_file("${devilsai_SOURCE_DIR}/config.h.cmake" "${devilsai_BINARY_DIR}/config.h" @ONLY)

find_package(SFML 2 COMPONENTS system window graphics audio REQUIRED)
find_package(PhysFS)
find_package(Lua52)
find_package(TinyXML2)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SFML_INCLUDE_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${TinyXML2_INCLUDE_DIRS}
    ${devilsai_SOURCE_DIR}
)

set(devilsai_SRCS
	Bibliotheque/Bibliotheque.h
	Bibliotheque/Constantes.h
	Bibliotheque/Decorations.cpp
	Bibliotheque/luaFunctions.cpp
    devilsai-gui/console.cpp
    devilsai-gui/conversation.cpp
    devilsai-gui/inventory.cpp
    devilsai-gui/journal.cpp
    devilsai-gui/storageBox.cpp
    devilsai-resources/Carte.cpp
    devilsai-resources/pack.cpp
    devilsai-resources/quests.cpp
    devilsai-resources/skill.cpp
    devilsai-resources/stats.cpp
    devilsai-resources/wearableItem.cpp
	gamedata.cpp
    skillbarManager.cpp
	ElementsCarte/Actionneurs.cpp
	ElementsCarte/Coffres.cpp
	ElementsCarte/Elements.cpp
	ElementsCarte/ElementsCarte.h
	ElementsCarte/GestionIndividus.cpp
	ElementsCarte/GestionJoueurs.cpp
	ElementsCarte/Individus.cpp
	ElementsCarte/IndividusCommuns.cpp
	ElementsCarte/IndividusUniques.cpp
	ElementsCarte/Joueurs.cpp
	ElementsCarte/Paysages.cpp
	Jeu/Jeu.cpp
	Jeu/Jeu.h
	Jeu/main.cpp
	Jeu/Options.cpp
	Jeu/Sauvegarde.cpp
)

set(devilsaiTargetLibs
    ${SFML_LIBRARIES}
    ${PHYSFS_LIBRARY}
    ${LUA_LIBRARIES}
    ${TinyXML2_LIBRARIES}
    ${sfml-all_LIBS}
)


add_subdirectory(tools)
add_subdirectory(imageManager)
add_subdirectory(gui)
add_subdirectory(musicManager)
add_subdirectory(textManager)

add_executable(devilsai ${devilsai_SRCS})

if (WIN32)
  add_executable(devilsai devilsai.rc)
endif (WIN32)

target_link_libraries(devilsai
    ${devilsaiTargetLibs}
    tools
    imageManager
    gui
    musicManager
    textManager
)

SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++17 -pthread -O3 -fopt-info-all=optimisations.txt" CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++17 -pthread -Wall -Wextra -Wshadow -Wpedantic -Wformat-nonliteral \
            -Wformat-security -Wswitch-default -Wuninitialized -Wundef \
            -Wpointer-arith -Woverloaded-virtual -Wctor-dtor-privacy \
            -Wnon-virtual-dtor -Woverloaded-virtual -Wsign-promo \
            -Wno-unused-parameter -Weffc++ -g" CACHE INTERNAL "" FORCE)

if (UNIX)
  set(exeDestination ${CMAKE_INSTALL_PREFIX}/bin)
else (UNIX)
  set(exeDestination ${CMAKE_INSTALL_PREFIX})
endif (UNIX)

install(TARGETS devilsai RUNTIME DESTINATION ${exeDestination})
