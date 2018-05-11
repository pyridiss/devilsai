--[[

Quest "First Demon" (birnam:demon)

Steps:
1 - Texts are displayed.
2 - The player must kill Stolas.
3 - The player must talk to Fluellen. The next quest is launched.

]]


-- global section
-- --------------

player_ptr   = 0
stolas_ptr   = 0
fluellen_ptr = 0

questStep = "0"


-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then

		player_ptr   = getElement("player")
		fluellen_ptr = getElement("fluellen")
        stolas_ptr   = getElement("stolas")

        if addNewElements == "true" then
            pushDialog("birnam", "dialog-demon-introduction")
            addJournalEntry("birnam", "demon", "journal-demon-title")
            addJournalEntry("birnam", "demon", "journal-demon-text1")
        end

		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then
		if get(stolas_ptr, "life") == 0 then
			questStep = "2"
-- END OF GAME
--			pushDialog("1-KillStolas-GoToCamp")
            pushDialog("birnam", "EndOfGame")
		end

-- END OF GAME
--	elseif questStep == "2" then
--		if interact(player_ptr, fluellen_ptr) then
--			questStep = "3"
--			pushDialog("1-KillStolas-Fluellen")
--		end

	end

end

function questIsDone()
--	return (questStep == "3")
-- END OF GAME
	return false
end

function questSave()
	return "" .. questStep
end

function questRecoverState(data)
	_, _, questStep = string.find(data, "(%d+)")

	if questStep == "2" then
-- END OF GAME
--		pushDialog("1-KillStolas-GoToCamp")
        pushDialog("birnam", "EndOfGame")
	end

end

function questEnd()
-- END OF GAME
--	addQuest("1-GowersWorries")
    addExperience(player_ptr, 5000)
end
