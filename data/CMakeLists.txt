add_subdirectory(carte)
add_subdirectory(chapitre)
add_subdirectory(img)
add_subdirectory(individu)
add_subdirectory(lng)
add_subdirectory(object)
add_subdirectory(quest)
add_subdirectory(skill)


set(data
    Cardinal.ttf
    DayRoman.ttf
    defaut.opt
    devilsai.png
)

install(FILES ${data} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/devilsai)

