--[[

Map 1 - Quest "Wizard"

]]


-- global section
-- --------------

player_ptr = 0
wizard_ptr = 0

checkPoint1 = 0
checkPoint2 = 0

questStep = "0"

remainingTime = 50

-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		if addNewElements == "true" then
            loadWorld("birnam", "quest/birnam/Wizard.xml", "Wizard-Horst")
		end

        -- Find Player and Horst
		player_ptr = getElement("player")
        wizard_ptr = getElement("horst")

        checkPoint1 = addCheckPoint("birnam", 1820, -5301, 10, 100)
        checkPoint2 = addCheckPoint("birnam", 1480, -2281, 10, 200)

		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then
		if interact(player_ptr, checkPoint1) == true or interact(player_ptr, checkPoint2) == true then
			deleteList("Actionneurs-Noirefontaine")
			pushDialog("1-Wizard-Beginning")
			questStep = "2"
		end

	elseif questStep == "2" then
		if interact(player_ptr, wizard_ptr) == true then
			pushDialog("1-Wizard-Wizard")
			questStep = "3"
		end

	elseif questStep == "3" then
		remainingTime = remainingTime - I()
		if remainingTime <= 0 then
			set(wizard_ptr, "vitality", 0)
            loadWorld("birnam", "quest/birnam/Wizard.xml", "Wizard-Monsters")
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
	addExperience(1500)
end
