--[[

Map 1 - Quest "Invasion of the Glade of the Saints"

Steps:
1 - The player must kill all monsters from list 1100. When there are less than 5 remaining monsters,
	this number is displayed on the screen.
2 - The player must talk to Gower. Texts are displayed.
3 - The next quest "Quest2" is launched and the player gains experience.
A - At the beginning, it must be written that the player should go talk to Gower.
B - If the player goes talk to Gower, texts are displayed.

]]


-- global section
-- --------------

player_ptr = 0
gower_ptr  = 0

questStep = "0"
hasTalkedToGower = false

remainingMonstersDisplayed = 0

-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		-- Find Player and Gower
		player_ptr = getElement("200") -- 200 = player
		gower_ptr  = getElement("201") -- 201 = gower

		if addNewElements == "true" then
			loadElement([[CARTE_MERE 1 LISTE_IMMUABLE
--							ACTIONNEUR 3652 -881
--							RECT_COL 10 100
--							TEXTE_PERSO_EST 2
--							FIN_ACTIONNEUR
--							PAYSAGE 30205 3752 -881]], "Obstacle-GladeSaints-IceRoad")
			loadElement("CARTE_MERE 1 IND_COMMUN 210 489 -33", "Warriors-GladeSaints")
			loadElement("CARTE_MERE 1 IND_COMMUN 210 580 -33", "Warriors-GladeSaints")
			loadElement("CARTE_MERE 1 IND_COMMUN 210 486  62", "Warriors-GladeSaints")
			loadElement("CARTE_MERE 1 IND_COMMUN 210 580  62", "Warriors-GladeSaints")
		end

		pushDialog("1-InvasionGlade-Intro1")
		addJournalEntry("1-InvasionGlade-Intro1")
		questStep = "1"
	end

end

function questManage()

	if hasTalkedToGower == false then
		if collisionCC(player_ptr, gower_ptr) then
			pushDialog("1-InvasionGlade-Intro2")
			journalEntryDone("1-InvasionGlade-Intro1")
			addJournalEntry("1-InvasionGlade-Intro2")
			hasTalkedToGower = true
		end
	end

	if questStep == "1" then
		remainingEnemies = getNumberEnemiesInList("1-GladeSaints-Monsters")
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
		if collisionCC(player_ptr, gower_ptr) then
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

	popDialog("1-InvasionGlade-Intro1")

	if questStep == "2" then
		pushDialog("1-InvasionGlade-1")
	elseif questStep == "3" then
		pushDialog("1-InvasionGlade-2")
	end
end

function questEnd()
	addQuest("1-RescueFluellensCamp")
	addQuest("1-UnknownStone")
	addExperience(1000)
end
