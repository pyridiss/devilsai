--[[

Map 1 - Quest "Free Ice Road"

]]


-- global section
-- --------------

player_ptr   = 0
gower_ptr    = 0
fluellen_ptr = 0

questStep = "0"

remainingMonsters = 1
timer = 0


-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		-- Find Player and Gower
		player_ptr   = getElement("player")
		gower_ptr    = getElement("gower")
		fluellen_ptr = getElement("fluellen")
		pushDialog("1-FreeIceRoad-Advice")

		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then

        if getTimeElapsed(timer) > 1000 then
            timer = resetTimer()
            remainingMonsters = getNumberOfItemsByTag("birnam", "IceRoad-Monsters")
        end

        if remainingMonsters == 0 then
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
            loadWorld("birnam", "quest/birnam/FreeIceRoad.xml", "FreeIceRoad-Reward")
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
