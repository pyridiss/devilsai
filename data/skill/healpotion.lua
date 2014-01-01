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

function setActiviteFinished(value)
	activiteFinished = value
end

function use()
	if activated == true and activiteFinished == true then
		useObject(owner, object)
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

function dispImage(x, y)
	dispImageSkill(getInternalNumber(), x, y)
	dispText(getQuantityOf(owner, object), x + 30, y + 30)
end