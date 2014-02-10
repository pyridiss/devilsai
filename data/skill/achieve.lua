--[[

Skill "Achieve"

]]


-- global section
-- --------------

owner = 0 -- This is a pointer to the owner of this skill

animation = 102

needsToBeActivated = true
activated          = false
activiteFinished   = false

degats    = 250
amplitude = 5

-- Owner loses energy during the whole activity
runEnergy = 10

partOfSkill = "run" -- 1 - "run" ; 2 - "attack"

-- We will not wait forever
numberOfSteps = 0
maxSteps = 3

activityRun = 0
activityAttack = 0
activityRest = 0

-- functions
-- ---------

function skillBegin(_owner)
	owner = _owner

	activityRun = createActivite(owner, 1020)
	activiteSet(activityRun, "speed", 30)
	activiteSet(activityRun, "step", 26)
	activiteSet(activityRun, "priority", 1)
	activiteSet(activityRun, "numberOfDir", 8)
	activiteAddImage(activityRun, 3, 0)
	activiteAddImage(activityRun, 3, 2)
	activiteAddImage(activityRun, 3, 4)
	activiteAddImage(activityRun, 3, 6)

	activityAttack = createActivite(owner, 1021)
	activiteSet(activityAttack, "speed", 30)
	activiteSet(activityAttack, "step", 26)
	activiteSet(activityAttack, "priority", 1)
	activiteSet(activityAttack, "numberOfDir", 8)
	activiteAddImage(activityAttack, 101, 2)
	activiteAddImage(activityAttack, 101, 4)
	activiteAddImage(activityAttack, 101, 6)
	activiteAddImage(activityAttack, 101, 8)
	activiteAddImage(activityAttack, 101, 10)

	activityRest = createActivite(owner, 1022)
	activiteSet(activityRest, "speed", 4)
	activiteSet(activityRest, "step", 0)
	activiteSet(activityRest, "priority", 2)
	activiteSet(activityRest, "numberOfDir", 8)
	activiteAddImage(activityRest, 2, 0)
	activiteAddImage(activityRest, 2, 2)
	activiteAddImage(activityRest, 2, 4)
	activiteAddImage(activityRest, 2, 6)

end

function setActivated(value)
	activated = value
	if activated == false then
		partOfSkill = "run"
		numberOfSteps = 0
	end
end

function getActivated()
	return activated
end

function setActiviteFinished(value)
	activiteFinished = value
end

function use()
	if activated == true then
		set(owner, "energy", get(owner, "energy") - I(runEnergy))

		enemy = getElementInteraction(owner)

		if enemy ~= 0 or numberOfSteps >= maxSteps then
			if partOfSkill == "run" then
				activiteFinished = false
				partOfSkill = "attack"
			end
		end

		if partOfSkill == "attack" and activiteFinished == true then --We wait for the end of attack animation
			if isIndividu(enemy) and collisionCC(owner, "RayonInteraction", enemy, "RayonCollision") and testAngle(owner, enemy) then
				combat(owner, enemy)
			end
			set(owner, "energy", get(owner, "energy") - getNeededEnergy())
			partOfSkill = "rest"
			activiteFinished = false
		end

		if partOfSkill == "rest" and activiteFinished == true then
			activated = false
			partOfSkill = "run"
			numberOfSteps = 0
		end
	end
	activiteFinished = false
	numberOfSteps = numberOfSteps + I(1)
end

function getActivite()
	if partOfSkill == "run" then
		return 1020
	elseif partOfSkill == "attack" then
		return 1021
	end
	return 1022
end

function getInternalNumber()
	return animation
end

function getDegats()
	return degats
end

function getAmplitude()
	return amplitude
end

function getNeededEnergy()
	return 2 * get(owner, "Puissance") / get(owner, "Constitution")
end

function testEnoughEnergy()

	energy = get(owner, "energy")

	if energy < 2 * getNeededEnergy() then
		return false
	else
		return true
	end

end

function getForce()
	return 0
end

function getPuissance()
	return 0
end

function getAgilite()
	return 0
end

function getIntelligence()
	return 0
end

function getConstitution()
	return 0
end

function getCharisme()
	return 0
end

function getEsquive()
	return 0
end

function getRecuperationMoyenne()
	return 0
end

function getMultForce()
	return 0
end

function getMultPuissance()
	return 0
end

function getMultAgilite()
	if activated == true then
		if partOfSkill == "rest" then
			return -100
		else
			return 10000
		end
	end
	return 0
end

function getMultIntelligence()
	return 0
end

function getMultConstitution()
	if activated == true then
		return -50
	end
	return 0
end

function getMultCharisme()
	return 0
end

function getMultEsquive()
	if activated == true then
		return -100
	end
	return 0
end

function getMultRecuperationMoyenne()
	return 0
end

function getVitesseCourse()
	if activated == true then
		return 100
	end
	return 0
end

function getVitesseAttaque()
	if activated == true then
		return 100
	end
	return 0
end

function getVitesseBlesse()
	return 0
end

function dispImage(x, y)
	dispImageSkill(getInternalNumber(), x, y)
end