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
runEnergy = 3

partOfSkill = "run" -- 1 - "run" ; 2 - "attack"

-- We will not wait forever
numberOfSteps = 0
maxSteps = 10

-- functions
-- ---------

function skillBegin(_owner)
	owner = _owner
end

function setActivated(value)
	activated = value
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
			partOfSkill = "attack"
		end

		if partOfSkill == "attack" and activiteFinished == true then --We wait for the end of attack animation
			if isIndividu(enemy) and collisionCC(owner, "RayonInteraction", enemy, "RayonCollision") and testAngle(owner, enemy) then
				combat(owner, enemy)
			end
			set(owner, "energy", get(owner, "energy") - getNeededEnergy())
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
		return 3
	end
	return 101 -- 101 is normal attack
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
	return 2 * get(owner, "power") / get(owner, "constitution");
end

function testEnoughEnergy()

	energy = get(owner, "energy")

	if (energy < 2 * getNeededEnergy()) then
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

function getRecuperation()
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
		return 10000
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

function getMultRecuperation()
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