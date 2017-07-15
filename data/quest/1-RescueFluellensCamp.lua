--[[

Map 1 - Quest "Rescue of Fluellen's Camp"

Steps:
1 - Texts are displayed.
2 - The player must reach one of the two Actionneurs.
3 - The player must kill monsters of the list "1-AncientLands-Monsters". If less than five monsters, this number is displayed.
4 - The player must talk to Fluellen. The next quest is launched.

]]


-- global section
-- --------------

player_ptr   = 0
gower_ptr    = 0
fluellen_ptr = 0

act_ptr_1 = 0
act_ptr_2 = 0

questStep = "0"

remainingMonstersDisplayed = 0

-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		-- Find Player, Gower and Fluellen
		player_ptr   = getElement("200") -- 200 = player
		gower_ptr    = getElement("gower")
		fluellen_ptr = getElement("202") -- 202 = fluellen

		-- Add new elements and monsters
		act_ptr_1  = addActionneur(2222, -5304, 35, 100)
		act_ptr_2  = addActionneur(2440, -3359, 100, 35)

		deleteList("Obstacle-GladeSaints-IceRoad")

		loadList("1-IceRoad-AncientLands-Lands1")
		loadList("1-IceRoad-AncientLands-Lands2")

		loadElement("PAYSAGE 10004 4592 -814", "Obstacle-IceRoad-Forres")
		loadElement("PAYSAGE 10004 4656 -814", "Obstacle-IceRoad-Forres")
		loadElement("PAYSAGE 10004 4720 -814", "Obstacle-IceRoad-Forres")

		loadElement("PAYSAGE 10104 4688 -3722", "Obstacle-AncientLands-Birnam")
		loadElement("PAYSAGE 10109 4793 -3718", "Obstacle-AncientLands-Birnam")
		loadElement("PAYSAGE 10108 4588 -3717", "Obstacle-AncientLands-Birnam")
		loadElement("PAYSAGE 10102 4560 -3671", "Obstacle-AncientLands-Birnam")
		loadElement("PAYSAGE 10102 4816 -3671", "Obstacle-AncientLands-Birnam")

		loadElement("PAYSAGE 10100 2622 -4429", "Gate-FluellensCamp")
		loadElement("PAYSAGE 10101 2622 -4355", "Gate-FluellensCamp")
		loadElement("PAYSAGE 10101 2622 -4259", "Gate-FluellensCamp")
		loadElement("PAYSAGE 10102 2622 -4196", "Gate-FluellensCamp")

		loadElement("ACTIONNEUR 1790 -5301	RECT_COL 10 100	TEXTE_PERSO_OUEST 1	FIN_ACTIONNEUR", "Actionneurs-Noirefontaine")
		loadElement("ACTIONNEUR 1850 -5301	RECT_COL 10 100	COLLISION_DIPLOM 2	FIN_ACTIONNEUR", "Actionneurs-Noirefontaine")
		loadElement("ACTIONNEUR 1450 -2281	RECT_COL 10 200	TEXTE_PERSO_OUEST 1	FIN_ACTIONNEUR", "Actionneurs-Noirefontaine")
		loadElement("ACTIONNEUR 1510 -2281	RECT_COL 10 200	COLLISION_DIPLOM 2	FIN_ACTIONNEUR", "Actionneurs-Noirefontaine")

		loadList("1-IceRoad-Monsters")
		loadList("1-AncientLands-Monsters")

		pushDialog("1-RescueFluellensCamp-Beginning")
		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then
		if interact(player_ptr, act_ptr_1) or interact(player_ptr, act_ptr_2) then
			deleteList("Warriors-GladeSaints")
			questStep = "2"
			pushDialog("1-RescueFluellensCamp-Reminder")
		end

	elseif questStep == "2" then
		remainingEnemies = getNumberEnemiesInList("1-AncientLands-Monsters")
		if remainingEnemies <= 5 and remainingEnemies > 0 then
			if remainingEnemies ~= remainingMonstersDisplayed then
				dispRemainingEnemies(remainingEnemies)
				remainingMonstersDisplayed = remainingEnemies
			end
		end
		if remainingEnemies == 0 then
			deleteList("Gate-FluellensCamp")
			questStep = "3"
			pushDialog("1-RescueFluellensCamp-GoToCamp")
		end

	elseif questStep == "3" then
		if interact(player_ptr, fluellen_ptr) then
			questStep = "4"
			pushDialog("1-RescueFluellensCamp-Fluellen")
		end

	elseif questStep == "4" then
		if dialogDisplayed("1-RescueFluellensCamp-Fluellen") then
			questStep = "5"
		end

	end

end

function questIsDone()
	return (questStep == "5")
end

function questSave()
	return questStep
end

function questRecoverState(data)
	_, _, questStep = string.find(data, "(%d+)")

	if questStep ~= "1" then
		popDialog("1-RescueFluellensCamp-Beginning")
	end

	if questStep == "2" then
		pushDialog("1-RescueFluellensCamp-Reminder")
	elseif questStep == "3" then
		pushDialog("1-RescueFluellensCamp-GoToCamp")
	end
end

function questEnd()
	deleteElement(act_ptr_1)
	deleteElement(act_ptr_2)
	deleteList("Obstacle-AncientLands-Birnam")
	addQuest("1-KillStolas")
	addQuest("1-FreeIceRoad")
	addQuest("1-Wizard")
	addExperience(2000)
end
