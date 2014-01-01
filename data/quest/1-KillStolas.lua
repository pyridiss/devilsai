--[[

Map 1 - Quest "Kill Stolas"

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
		if addNewElements == "true" then
			loadList("1-Birnam-Lands1")
			loadList("1-Birnam-Lands2")
			loadList("1-Birnam-Monsters")
		end

		player_ptr   = getElement("200")
		fluellen_ptr = getElement("202")
		stolas_ptr   = getElement("400")

		questStep = "1"
		pushDialog("1-KillStolas-Beginning")
	end

end

function questManage()

	if questStep == "1" then
		if get(stolas_ptr, "vitality") == 0 then
			questStep = "2"
-- END OF GAME
--			pushDialog("1-KillStolas-GoToCamp")
			pushDialog("EndOfGame")
		end

-- END OF GAME
--	elseif questStep == "2" then
--		if collisionCC(player_ptr, fluellen_ptr) then
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
		popDialog("1-KillStolas-Beginning")
-- END OF GAME
--		pushDialog("1-KillStolas-GoToCamp")
		pushDialog("EndOfGame")
	end

end

function questEnd()
-- END OF GAME
--	addQuest("1-GowersWorries")
	addExperience(5000)
end
