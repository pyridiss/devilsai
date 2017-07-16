--[[

Map 1 - Quest "Gower's Worries"

Steps:
1 - Fluellen says that the player should go talk to Gower. The player reaches the Actionneur. Enemies are loaded.
2 - Either Gower is killed (jump to Step 10), or all enemies are killed (jump to Step 20)
10 - Gower says the player must go to Forres. Next quest.
20 - The player must go talk to Fluellen.
21 - Fluellen says the player must go to Forres.
22 - Next quest.

]]


-- global section
-- --------------

player_ptr   = 0
gower_ptr    = 0
fluellen_ptr = 0

act_ptr = 0

questStep = "0"


-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then

		player_ptr   = getElement("player")
		gower_ptr    = getElement("gower")
		fluellen_ptr = getElement("202")

		act_ptr = addActionneur(3710, -880, 10, 100)

		pushDialog("mis_1.10_intro") -- TODO Find a better name

		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then
		if interact(player_ptr, act_ptr) then
			loadList("1004")
			loadList("1105")
			questStep = "2"
		end

	elseif questStep == "2" then
		if getNumberEnemiesInList("1105") == 0 then
			deleteList("Obstacle-IceRoad-Forres")
			loadList("1241")
			addExperience(4000)
			pushDialog("mis_1.13_ccl")
			pushDialog("mis_1.14_intro")
			questStep = "10"
		end
		if get(gower_ptr, "vitality") == 0 then
			questStep = "20"
		end

	--elseif questStep == 10 then
		-- Auparavant : Lancement de F1, G1, H1, et 13 !

	elseif questStep == "20" then
		if interact(player_ptr, gower_ptr) then
			pushDialog("mis_1.E3_intro")
			questStep = "21"
		end

	elseif questStep == "21" then
		if interact(player_ptr, fluellen_ptr) then
			deleteList("Obstacle-IceRoad-Forres")
			loadList("1241")
			pushDialog("mis_1.E3_ccl")
			pushDialog("mis_1.E4_intro")
			questStep = "22"
		end
	end

end

function questIsDone()
	return (questStep == "10" or questStep == "22")
end

function questSave()
	return ""
end

function questRecoverState(data)
end

function questEnd()
	deleteElement(act_ptr)
	addExperience(1000)
end
