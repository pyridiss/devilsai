project(devilsai)

set(VERSION "master")
string(TIMESTAMP TIME %Y-%m-%d)

configure_file("${devilsai_SOURCE_DIR}/config.h.cmake" "${devilsai_BINARY_DIR}/config.h" @ONLY)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
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
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
    physfs
    lua5.2
    tinyxml2
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

set_target_properties(devilsai PROPERTIES COMPILE_FLAGS "-std=c++11")

install(TARGETS devilsai RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
