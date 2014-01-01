--[[

Skill "Attack"

]]


-- global section
-- --------------

owner = 0 -- This is a pointer to the owner of this skill

animation = 101

needsToBeActivated = true
activated          = false
activiteFinished   = false

degats    = 180
amplitude = 20


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
	return get(owner, "power") / get(owner, "constitution");
end

function testEnoughEnergy()

	energy = get(owner, "energy")

	if (energy < 2 * getNeededEnergy()) then
		return false
	else
		return true
	end

end

function dispImage(x, y)
	dispImageSkill(getInternalNumber(), x, y)
end