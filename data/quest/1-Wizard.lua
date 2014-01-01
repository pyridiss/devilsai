--[[

Map 1 - Quest "Wizard"

]]


-- global section
-- --------------

player_ptr = 0
wizard_ptr = 0

act_ptr_1 = 0
act_ptr_2 = 0

questStep = "0"

remainingTime = 50

-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		if addNewElements == "true" then
			loadElement("IND_UNIQUE 205 1426 -5711", "DEFAUT")
		end

		-- Find Player and Gower
		player_ptr = getElement("200") -- 200 = player
		wizard_ptr = getElement("205") -- 205 = wizard

		act_ptr_1 = addActionneur(1820, -5301, 10, 100)
		act_ptr_2 = addActionneur(1480, -2281, 10, 200)

		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then
		if collisionCR(player_ptr, act_ptr_1) == true or collisionCR(player_ptr, act_ptr_2) == true then
			deleteList("Actionneurs-Noirefontaine")
			pushDialog("1-Wizard-Beginning")
			questStep = "2"
		end

	elseif questStep == "2" then
		if collisionCC(player_ptr, wizard_ptr) == true then
			pushDialog("1-Wizard-Wizard")
			questStep = "3"
		end

	elseif questStep == "3" then
		remainingTime = remainingTime - I()
		if remainingTime <= 0 then
			set(wizard_ptr, "vitality", 0)
			loadList("1-Noirefontaine-Monsters")
			questStep = "4"
		end
	end
end

function questIsDone()
	return (questStep == "4")
end

function questSave()
	return questStep .. " " .. math.floor(remainingTime)
end

function questRecoverState(data)
	_, _, questStep, remainingTime = string.find(data, "(%d+) (%d+)")

	if questStep == "2" then
		pushDialog("1-Wizard-Beginning")
	end
end

function questEnd()
	deleteElement(act_ptr_1)
	deleteElement(act_ptr_2)
	addExperience(1500)
end
