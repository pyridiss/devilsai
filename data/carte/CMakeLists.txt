
set(carte
    1-AncientLands-Monsters.lst
    1-Birnam-Lands1.lst
    1-Birnam-Lands2.lst
    1-Birnam-Monsters.lst
    1-IceRoad-AncientLands-Lands1.lst
    1-IceRoad-AncientLands-Lands2.lst
    1-IceRoad-Monsters.lst
    1-Noirefontaine-Monsters.lst
    1-OutsideLands.lst
    1-ways.lst
    101.map
	1004.lst
	1105.lst
	1241.lst
	101107.lst
	101200.lst
    birnam.xml
	didact_coffre.lst
    tutorial.xml
)

install(FILES ${carte} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/devilsai/carte)

