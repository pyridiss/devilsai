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

-- functions
-- ---------

function skillBegin(_owner)
	owner = _owner
end

function setActivated(value)
	activated = value
end

function setActiviteFinished(value)
	activiteFinished = value
end

function use()
	if activated == true then
		set(owner, "energy", get(owner, "energy") - I(runEnergy))
	end

	if activated == true and activiteFinished == true then
		enemy = getElementInteraction(owner)
		if enemy ~= 0 and testEnoughEnergy() == true then
			if isIndividu(enemy) and collisionCC(owner, "RayonInteraction", enemy, "RayonCollision") and testAngle(owner, enemy) then
				combat(owner, enemy);
			end
		end
		set(owner, "energy", get(owner, "energy") - getNeededEnergy());
	end
	activiteFinished = false
end

function getActivite()
	return animation
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

function dispImage(x, y)
	dispImageSkill(getInternalNumber(), x, y)
end