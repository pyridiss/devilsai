--[[

Map 1 - Quest "Rescue of Fluellen's Camp"

Steps:
1 - Texts are displayed.
2 - The player must reach one of the two check points.
3 - The player must kill monsters of the list "1-AncientLands-Monsters". If less than five monsters, this number is displayed.
4 - The player must talk to Fluellen. The next quest is launched.

]]


-- global section
-- --------------

player_ptr   = 0
gower_ptr    = 0
fluellen_ptr = 0

checkPoint1 = 0 -- AncientLands entrance
checkPoint2 = 0 -- AncientLands entrance
checkPoint3 = 0 -- Noirefontaine entrance
checkPoint4 = 0 -- Noirefontaine entrance

questStep = "0"

remainingMonstersDisplayed = 0

-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		-- Find Player, Gower and Fluellen
		player_ptr   = getElement("player")
		gower_ptr    = getElement("gower")
		fluellen_ptr = getElement("fluellen")

		-- Add new elements and monsters
        checkPoint1 = addCheckPoint("birnam", 2222, -5304, 35, 100)
        checkPoint2 = addCheckPoint("birnam", 2440, -3359, 100, 35)
        checkPoint3 = addCheckPoint("birnam", 1790, -5301, 35, 100)
        checkPoint4 = addCheckPoint("birnam", 1450, -2281, 35, 200)

		deleteList("Obstacle-GladeSaints-IceRoad")

		loadList("1-IceRoad-AncientLands-Lands1")
        loadWorld("birnam", "carte/birnam.xml", "IceRoad-InertItems")

        loadWorld("birnam", "quest/birnam/RescueFluellensCamp.xml", "Obstacle-AncientLands-Birnam")
        loadWorld("birnam", "quest/birnam/RescueFluellensCamp.xml", "Gate-FluellensCamp")
        loadWorld("birnam", "quest/birnam/RescueFluellensCamp.xml", "RescueFluellensCamp-ObstacleForEnemies")

        loadWorld("birnam", "carte/birnam.xml", "IceRoad-Monsters")
		loadList("1-AncientLands-Monsters")

		pushDialog("1-RescueFluellensCamp-Beginning")
		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then
		if interact(player_ptr, checkPoint1) or interact(player_ptr, checkPoint2) then
			deleteList("Warriors-GladeSaints")
			questStep = "2"
			pushDialog("1-RescueFluellensCamp-Reminder")
		end

	elseif questStep == "2" then
		remainingEnemies = getNumberOfItemsByTag("birnam", "1-AncientLands-Monsters")
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

    if interact(player_ptr, checkPoint3) or interact(player_ptr, checkPoint4) then
        pushDialog("actionneur_1_1")
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
	deleteList("Obstacle-AncientLands-Birnam")
	addQuest("1-KillStolas")
	addQuest("1-FreeIceRoad")
	addQuest("1-Wizard")
	addExperience(2000)
end
