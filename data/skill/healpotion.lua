--[[

Skill "Attack"

]]


-- global section
-- --------------

owner = 0 -- This is a pointer to the owner of this skill

animation = 401

needsToBeActivated = true
activated          = false
activiteFinished   = false

object = "healpotion"


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
	if activated == true and activiteFinished == true then
		useObject(owner, object)
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

function getNeededEnergy()
	return 0
end

function testEnoughEnergy()
	return true
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

function dispImage(x, y)
	dispImageSkill(getInternalNumber(), x, y)
	dispText(getQuantityOf(owner, object), x + 30, y + 30)
end