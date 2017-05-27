
set(gui
    confirm-exit-game.xml
    ingame-menu.xml
    ingame-toolbar.xml
    loading.xml
    main-menu.xml
    new-game.xml
    player-dead.xml
)

install(FILES ${gui} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/devilsai/gui)

