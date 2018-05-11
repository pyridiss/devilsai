--[[

Quest "Tortured mind" (birnam:mind)

]]


-- global section
-- --------------

player_ptr = 0
wizard_ptr = 0

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

		questStep = "1"
	end

end

function questManage()

	elseif questStep == "2" then
		if interact(player_ptr, wizard_ptr) == true then
            pushDialog("birnam", "dialog-mind-wizard")
			questStep = "3"
		end

	elseif questStep == "3" then
		remainingTime = remainingTime - I()
		if remainingTime <= 0 then
			set(wizard_ptr, "life", 0)
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
end

function questEnd()
    addExperience(player_ptr, 1500)
end
