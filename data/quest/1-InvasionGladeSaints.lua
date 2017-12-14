--[[

Quest "Invasion of the Glade of the Saints"

Summary:
  The player must kill all monsters with tag "TheGladeOfTheSaints-Monsters".
  When there are less than 5 remaining monsters, this number is displayed on the screen.
  When the quest is done, the player gains experience.

]]


-- global section
-- --------------

player_ptr = 0
gower_ptr  = 0

checkPoint = 0

questStep = "0"
hasTalkedToGower = false
hasReachedTheCheckPoint = false

remainingMonsters = 1
remainingMonstersDisplayed = 0

timer = 0

-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then

		player_ptr = getElement("player")
		gower_ptr  = getElement("gower")

		if addNewElements == "true" then
            loadWorld("birnam", "quest/birnam/FreeIceRoad.xml", "Obstacle-GladeSaints-IceRoad")
			pushDialog("1-InvasionGlade-Intro1")
			addJournalEntry("1-InvasionGlade-Intro1")
		end

		questStep = "1"
	end

    checkPoint = addCheckPoint("birnam", 3652, -880, 10, 100)

end

function questManage()

	if hasTalkedToGower == false then
		if interact(player_ptr, gower_ptr) then
			pushDialog("1-InvasionGlade-Intro2")
			journalEntryDone("1-InvasionGlade-Intro1")
			addJournalEntry("1-InvasionGlade-Intro2")
			hasTalkedToGower = true
		end
	end

    if hasReachedTheCheckPoint == false and interact(player_ptr, checkPoint) then
        pushDialog("actionneur_1_2")
        hasReachedTheCheckPoint = true
    end

	if questStep == "1" then

        if getTimeElapsed(timer) > 1000 then
            timer = resetTimer()
            remainingMonsters = getNumberOfItemsByTag("birnam", "TheGladeOfTheSaints-Monsters")
        end

        if remainingMonsters <= 5 and remainingMonsters > 0 then
            if remainingMonsters ~= remainingMonstersDisplayed then
                dispRemainingEnemies(remainingMonsters)
                remainingMonstersDisplayed = remainingMonsters
			end
		end
        if remainingMonsters == 0 then
			questStep = "2"
			pushDialog("1-InvasionGlade-1")
			journalEntryDone("1-InvasionGlade-Intro2")
			addJournalEntry("1-InvasionGlade-1")
		end

	elseif questStep == "2" then
		if interact(player_ptr, gower_ptr) then
			questStep = "3"
			pushDialog("1-InvasionGlade-2")
			journalEntryDone("1-InvasionGlade-1")
			addJournalEntry("1-InvasionGlade-2")
		end

	elseif questStep == "3" then
		if dialogDisplayed("1-InvasionGlade-2") then
			questStep = "4"
		end

	end

end

function questIsDone()
	return (questStep == "4")
end

function questSave()
	save = questStep .. " "
	if hasTalkedToGower == true then
		save = save .. "true"
	else
		save = save .. "false"
	end

	return save
end

function questRecoverState(data)
	_, _, questStep, hasTalkedToGower = string.find(data, "(%d+) (%a+)")

	if hasTalkedToGower == "true" then
		hasTalkedToGower = true
	else
		hasTalkedToGower = false
	end
end

function questEnd()
	addQuest("quest/1-RescueFluellensCamp.lua")
	addQuest("quest/1-UnknownStone.lua")
	addExperience(1000)
end
