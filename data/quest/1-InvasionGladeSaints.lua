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

remainingMonstersDisplayed = 0

-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then

		player_ptr = getElement("player")
		gower_ptr  = getElement("gower")

		if addNewElements == "true" then
            loadWorld("birnam", "carte/birnam.xml", "Obstacle-GladeSaints-IceRoad")
            loadWorld("birnam", "carte/birnam.xml", "TheGladeOfTheSaints-Monsters")
            loadWorld("birnam", "carte/birnam.xml", "Warriors-GladeSaints")
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
		remainingEnemies = getNumberOfItemsByTag("birnam", "TheGladeOfTheSaints-Monsters")
		if remainingEnemies <= 5 and remainingEnemies > 0 then
			if remainingEnemies ~= remainingMonstersDisplayed then
				dispRemainingEnemies(remainingEnemies)
				remainingMonstersDisplayed = remainingEnemies
			end
		end
		if remainingEnemies == 0 then
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
