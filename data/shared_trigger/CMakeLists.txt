
set(shared_trigger
    door.lua
    heal.lua
)

install(FILES ${shared_trigger} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/devilsai/shared_trigger)

