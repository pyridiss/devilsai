--[[

Quest "Trade Route" (birnam:route)

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

        if addNewElements == "true" then
            pushDialog("birnam", "dialog-route-advice")
        end

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
            addJournalEntry("birnam", "route", "journal-route-title")
            addJournalEntry("birnam", "route", "journal-route-text1")
			questStep = "2"
		end

	elseif questStep == "2" then
		if interact(player_ptr, fluellen_ptr) == true then
            pushDialog("birnam", "dialog-route-goseegower")
            addJournalEntry("birnam", "route", "journal-route-text2")
			questStep = "3"
		end

	elseif questStep == "3" then
		if interact(player_ptr, gower_ptr) == true then
            loadWorld("birnam", "quest/birnam/FreeIceRoad.xml", "FreeIceRoad-Reward")
            pushDialog("birnam", "dialog-route-reward")
            addJournalEntry("birnam", "route", "journal-route-text3")
            addJournalEntry("birnam", "route", "journal-route-end")
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
end

function questEnd()
    addExperience(player_ptr, 1500)
end
