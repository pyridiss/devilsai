add_subdirectory(carte)
add_subdirectory(chapitre)
add_subdirectory(gui)
add_subdirectory(img)
add_subdirectory(individu)
add_subdirectory(lng)
add_subdirectory(music)
add_subdirectory(object)
add_subdirectory(quest)
add_subdirectory(shared_trigger)
add_subdirectory(skill)
add_subdirectory(sound)


set(data
    LiberationSans-Regular.ttf
    LiberationSans-Bold.ttf
    Cardinal.ttf
    DayRoman.ttf
    defaut.opt
    devilsai.png
    blurShader.frag
    contrastShader.frag
)

install(FILES ${data} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/devilsai)

