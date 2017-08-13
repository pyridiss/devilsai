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
    ${LUA_INCLUDE_DIR}
    ${TinyXML2_INCLUDE_DIRS}
    ${devilsai_SOURCE_DIR}
)

set(devilsai_SRCS
    Attributs/Attributs.cpp
	Attributs/Attributs.h
	Attributs/Objets.cpp
	Bibliotheque/Bibliotheque.h
	Bibliotheque/Constantes.h
	Bibliotheque/Decorations.cpp
	Bibliotheque/Langues.cpp
	Bibliotheque/Templates.h
	Bibliotheque/Utilitaires.cpp
	Bibliotheque/luaFunctions.cpp
	Carte/Carte.cpp
	Carte/Carte.h
	Carte/Collisions.cpp
	Carte/Interactions.cpp
	gamedata.cpp
    inventoryScreenManager.cpp
    skillbarManager.cpp
    storageBoxScreenManager.cpp
	ElementsCarte/Actionneurs.cpp
	ElementsCarte/ChargementElements.cpp
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
	ElementsCarte/Projectiles.cpp
	Jeu/Jeu.cpp
	Jeu/Jeu.h
	Jeu/main.cpp
	Jeu/Missions.cpp
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

add_executable(devilsai ${devilsai_SRCS})

target_link_libraries(devilsai
    ${devilsaiTargetLibs}
    tools
    imageManager
    gui
    musicManager
)

SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -O3" CACHE INTERNAL "" FORCE)
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -Wall -g" CACHE INTERNAL "" FORCE)

install(TARGETS devilsai RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
