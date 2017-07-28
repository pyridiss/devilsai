--[[

Skill "Fire Ball"

]]


-- global section
-- --------------

owner = 0 -- This is a pointer to the owner of this skill

animation = 301

needsToBeActivated = true
activated          = false
activiteFinished   = false
projectile_puissance = 100
projectile_cout_fixe = 5


-- functions
-- ---------

function skillBegin(_owner)
	owner = _owner
	addSound("Fireball")
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
	if activated == true and activiteFinished == true and testEnoughEnergy() == true then

        angle = individual_get(owner, "angle")
        x = individual_get(owner, "PosX") + 20 * math.cos(angle)
        y = individual_get(owner, "PosY") + 20 * math.sin(angle)
        bullet = createIndividual("player_fireball_bullet", x, y)
        individual_copy(owner, bullet, "diplomacy")
        individual_copy(owner, bullet, "angle")

		set(owner, "energy", get(owner, "energy") - getNeededEnergy())
		activated = false

		playSound("Fireball")
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
	return projectile_puissance / get(owner, "constitution") + projectile_cout_fixe
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
	return "fireball"
end

function getImageFile()
    return "img/C301.png"
end
