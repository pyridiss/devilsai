--[[

Map 1 - Quest "Unknown Stone"

]]


-- global section
-- --------------

player_ptr = 0
gower_ptr  = 0

checkPoint1 = 0

questStep = "0"
hasTheStone = false
gowersWorriesLaunched = false
gowersWorriesDone     = false


-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		-- Find Player and Gower
		player_ptr = getElement("player")
		gower_ptr  = getElement("gower")

        checkPoint1 = addCheckPoint("birnam", 153, 125, 10, 10)

		questStep = "1"
	end

end

function questManage()

	if gowersWorriesLaunched == false and questRunning("1-GowersWorries") == true then
		gowersWorriesLaunched = true
	elseif gowersWorriesLaunched == true and questRunning("1-GowersWorries") == false then
		gowersWorriesDone = true
	end

	if questStep == "1" then
		if hasTheStone == false and possess(player_ptr, 700) == true then
			hasTheStone = true
			pushDialog("1-UnknownStone-Discovered")
		elseif hasTheStone == true and possess(player_ptr, 700) == false then
			hasTheStone = false
		end

		if hasTheStone == true and interact(player_ptr, gower_ptr) == true and gowersWorriesLaunched == false then
			transferObject(player_ptr, gower_ptr, 700)
			hasTheStone = false
			questStep = "2"
			pushDialog("1-UnknownStone-Given")
		end

		if hasTheStone == false and gowersWorriesDone == true then
            loadWorld("birnam", "quest/birnam/UnknownStone.xml", "UnknownStone-Scroll")
			questStep = "10"
		end

	elseif questStep == "2" then
		if gowersWorriesDone == true and interact(player_ptr, gower_ptr) == true then
			transferObject(gower_ptr, player_ptr, 700)
			pushDialog("1-UnknownStone-GivenBack")
			questStep = "3"
		end

	elseif questStep == "10" then
		if interact(player_ptr, checkPoint1) == true then
			deleteList("UnknownStone-Scroll")
			pushDialog("1-UnknownStone-Scroll")
			questStep = "11"
		end

	elseif questStep == "11" then
		if interact(player_ptr, gower_ptr) == true then
			pushDialog("1-UnknownStone-Gower")
			questStep = "12"
		end

	elseif questStep == "12" then
		if possess(player_ptr, 700) == true then
			pushDialog("1-UnknownStone-End")
			questStep = "13"
		end
	end

end

function questIsDone()
	return (questStep == "3" or questStep == "13")
end

function questSave()
	save = questStep .. " "

	if hasTheStone == true then
		save = save .. "true "
	else
		save = save .. "false "
	end

	if gowersWorriesLaunched == true then
		save = save .. "true "
	else
		save = save .. "false "
	end

	if gowersWorriesDone == true then
		save = save .. "true"
	else
		save = save .. "false"
	end

	return save
end

function questRecoverState(data)
	_, _, questStep, hasTheStone, gowersWorriesLaunched, gowersWorriesDone = string.find(data, "(%d+) (%g+) (%g+) (%g+)")

	if hasTheStone == "true" then
		hasTheStone = true
	else
		hasTheStone = false
	end

	if gowersWorriesLaunched == "true" then
		gowersWorriesLaunched = true
	else
		gowersWorriesLaunched = false
	end

	if gowersWorriesDone == "true" then
		gowersWorriesDone = true
	else
		gowersWorriesDone = false
	end

	if questStep == "11" then
		pushDialog("1-UnknownStone-Scroll")
	elseif questStep == "12" then
		pushDialog("1-UnknownStone-Gower")
	end
end

function questEnd()
	addExperience(2000)
end
