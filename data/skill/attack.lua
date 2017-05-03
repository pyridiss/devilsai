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
	math.randomseed(os.time())
	addSound("Sword_1")
	addSound("Sword_2")
	addSound("Sword_3")
	addSound("Sword_4")
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
	if activated == true and activiteFinished == true then
		enemy = getElementInteraction(owner)
		if enemy ~= 0 and testEnoughEnergy() == true then
			randomNumber = math.random(1, 4)
			playSound("Sword_" .. randomNumber)
			if isIndividu(enemy) and collisionCC(owner, "RayonInteraction", enemy, "RayonCollision") and testAngle(owner, enemy) then
				combat(owner, enemy)
			end
		end
		set(owner, "energy", get(owner, "energy") - getNeededEnergy())
		activated = false
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
	return get(owner, "Puissance") / get(owner, "Constitution");
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
	return 0
end

function getMultIntelligence()
	return 0
end

function getMultConstitution()
	return 0
end

function getMultCharisme()
	return 0
end

function getMultEsquive()
	return 0
end

function getMultRecuperationMoyenne()
	return 0
end

function getVitesseCourse()
	return 0
end

function getVitesseAttaque()
	return 0
end

function getVitesseBlesse()
	return 0
end

function getName()
	return "attack"
end

function getImageFile()
    return "img/C101.png"
end
