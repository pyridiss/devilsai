
set(carte
    1-AncientLands-Monsters.lst
    1-Birnam-Lands1.lst
    1-Birnam-Lands2.lst
    1-Birnam-Monsters.lst
    1-GladeSaints-Lands1.lst
    1-GladeSaints-Lands2.lst
    1-GladeSaints-Monsters.lst
    1-IceRoad-AncientLands-Lands1.lst
    1-IceRoad-AncientLands-Lands2.lst
    1-IceRoad-Monsters.lst
    1-Noirefontaine-Monsters.lst
    1-OutsideLands.lst
	1.map
    101.map
	1000.lst
	1004.lst
	1105.lst
	1241.lst
	101107.lst
	101200.lst
	didact.map
	didact_attaque.lst
	didact_bael.lst
	didact_coffre.lst
	didact_manneq1.lst
	didact_manneq2.lst
	didact_manneq3.lst
	didact_pay.lst
	didact_perso.lst
)

install(FILES ${carte} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/devilsai/carte)

