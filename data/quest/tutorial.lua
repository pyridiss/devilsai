--[[

Tutorial

Steps:
1 - Wait during 3 seconds.
2 - Texts are displayed. Wait for the end to load first mannequins.
3 - 
4 - 
5 - 

]]


-- global section
-- --------------

player_ptr = 0
curtis_ptr = 0
bael_ptr   = 0

remainingTime1 = 3
remainingTime2 = 5
remainingTime3 = 5
remainingTime4 = 10
remainingTime5 = 5
remainingTime6 = 5

questStep = "0"

-- functions
-- ---------

function questBegin(addNewElements)

	if questStep == "0" then
		player_ptr = getElement("tutorial-player")
		curtis_ptr = getElement("tutorial-curtis")
		questStep = "1"
	end

end

function questManage()

	if questStep == "1" then
		remainingTime1 = remainingTime1 - I()
		if remainingTime1 <= 0 then
			questStep = "2"
			pushDialog("tutorial-01")
		end

	elseif questStep == "2" then
		if dialogDisplayed("tutorial-01") then
            loadWorld("tutorial", "carte/tutorial.xml", "tutorial-training-1")
			pushDialog("tutorial-02")
			questStep = "3"
		end

	elseif questStep == "3" then
		if getNumberEnemiesInList("tutorial-training-1") == 0 and dialogDisplayed("tutorial-02") then
            loadWorld("tutorial", "carte/tutorial.xml", "tutorial-training-2")
			pushDialog("tutorial-03")
			pushDialog("tutorial-04")
			questStep = "4"
		end

	elseif questStep == "4" then
		if getNumberEnemiesInList("tutorial-training-2") == 0 and dialogDisplayed("tutorial-04") then
            loadWorld("tutorial", "carte/tutorial.xml", "tutorial-training-3")
			pushDialog("tutorial-05")
			pushDialog("tutorial-06")
			questStep = "5"
		end

	elseif questStep == "5" then
		if getNumberEnemiesInList("tutorial-training-3") == 0 and dialogDisplayed("tutorial-06") then
			pushDialog("tutorial-07")
			pushDialog("tutorial-08")
			questStep = "6"
		end

	elseif questStep == "6" then
		if dialogDisplayed("tutorial-08") then
			loadList("didact_coffre")
			pushDialog("tutorial-09")
			pushDialog("tutorial-10")
			questStep = "7"
		end

	elseif questStep == "7" then
		if dialogDisplayed("tutorial-10") then
			loadList("didact_attaque")
			enableCinematics(true)
			questStep = "8"
		end

	elseif questStep == "8" then
		remainingTime2 = remainingTime2 - I()
		if remainingTime2 <= 0 then
			deleteList("didact_attaque")
			loadList("didact_bael")
			bael_ptr = getElement("didact_bael")
			set(curtis_ptr, "vitality", 0)
			questStep = "9"
			pushDialog("tutorial-11")
		end

	elseif questStep == "9" then
		if dialogDisplayed("tutorial-11") then
			loadList("didact_attaque")
			setActivity(bael_ptr, 4)
			questStep = "10"
		end

	elseif questStep == "10" then
		remainingTime3 = remainingTime3 - I()
		if remainingTime3 <= 0 then
			deleteList("didact_attaque")
			questStep = "11"
			pushDialog("tutorial-12")
		end

	elseif questStep == "11" then
		if dialogDisplayed("tutorial-12") then
			setActivity(bael_ptr, 6)
			questStep = "12"
		end

	elseif questStep == "12" then
		remainingTime4 = remainingTime4 - I()
		if remainingTime4 <= 0 then
			setActivity(player_ptr, "falling")
			questStep = "13"
		end

	elseif questStep == "13" then
        setActivity(player_ptr, "lying-down")
		remainingTime5 = remainingTime5 - I()
		if remainingTime5 <= 0 then
			toBlack(true)
			questStep = "14"
		end

	elseif questStep == "14" then
		remainingTime6 = remainingTime6 - I()
		if remainingTime6 <= 0 then
			pushDialog("tutorial-13")
			questStep = "15"
		end

	elseif questStep == "15" then
		if dialogDisplayed("tutorial-13") then
			enableCinematics(false)
			toBlack(false)
			questStep = "16"
		end

	end

end

function questIsDone()
	return (questStep == "16")
end

function questSave()
	return "none"
end

function questRecoverState(data)

end

function questEnd()
	loadFirstChapter()
end
