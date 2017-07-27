
set(gui
    confirm-exit-game.xml
    ingame-menu.xml
    ingame-toolbar.xml
    inventory.xml
    load-game.xml
    loading.xml
    main-menu.xml
    new-game.xml
    options.xml
    player-dead.xml
)

install(FILES ${gui} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/devilsai/gui)

