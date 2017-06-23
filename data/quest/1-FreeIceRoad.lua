--[[

Map 1 - Quest "Free Ice Road"

]]


-- global section
-- --------------

player_ptr   = 0
gower_ptr    = 0
fluellen_ptr = 0

questStep = "0"


-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		-- Find Player and Gower
		player_ptr   = getElement("200") -- 200 = player
		gower_ptr    = getElement("201") -- 201 = gower
		fluellen_ptr = getElement("202") -- 202 = fluellen
		pushDialog("1-FreeIceRoad-Advice")

		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then
		if getNumberEnemiesInList("1-IceRoad-Monsters") == 0 then
			pushDialog("1-FreeIceRoad-GoToFluellen")
			questStep = "2"
		end

	elseif questStep == "2" then
		if interact(player_ptr, fluellen_ptr) == true then
			pushDialog("1-FreeIceRoad-GoToGower")
			questStep = "3"
		end

	elseif questStep == "3" then
		if interact(player_ptr, gower_ptr) == true then
			loadElement([[	COFFRE -81 -169
							RAY_COL 1
							NOM 612
							NOMBRE_OBJETS 1
							OBJET
							TYPE_ALEATOIRE spatha 9
							TYPE_ALEATOIRE pavis 9
							TYPE_ALEATOIRE bascinet 9
							TYPE_ALEATOIRE platearmor 9
							TYPE_ALEATOIRE fabricboots 9
							TYPE_ALEATOIRE sapphirering 9
							FIN_OBJET
							FIN_COFFRE]], "DEFAUT")
			loadElement("PAYSAGE 21002 -81 -169", "DEFAUT")
			pushDialog("1-FreeIceRoad-Reward")
			questStep = "4"
		end
	end
end

function questIsDone()
	return (questStep == "4")
end

function questSave()
	return "" .. questStep
end

function questRecoverState(data)
	_, _, questStep = string.find(data, "(%d+)")

	popDialog("1-FreeIceRoad-Advice")

	if questStep == "2" then
		pushDialog("1-FreeIceRoad-GoToFluellen")
	elseif questStep == "3" then
		pushDialog("1-FreeIceRoad-GoToGower")
	end
end

function questEnd()
	addExperience(1500)
end
