--[[

Object "Attack Potion" - (601.obj)

]]


-- global section
-- --------------

internalNumber = 601

categoryObject = "temporaire"
typeObject     = "potion"
classObject    = "potion"

key = 0

multForce        = 10
multPuissance    = 10
multAgilite      = 10
multIntelligence = 5

duree    = 7200
cumul    = true
quantite = 1

descriptionManuelle    = true
descriptionAutomatique = true


-- functions
-- ---------

function getCategorieObjet()
	return categoryObject
end

function getTypeObject()
	return typeObject
end

function getIdEmplacement()
	return categoryObject .. "-" .. typeObject
end

function getInternalNumber()
	return internalNumber
end

function getFileName()
	return "attackpotion"
end

function setKey(value)
	key = value
end

function getDuree()
	return duree
end

function setDuree(value)
	duree = value
end

function getDescriptionManuelle()
	return descriptionManuelle
end

function getDescriptionAutomatique()
	return descriptionAutomatique
end

function getForce()
	return 0
end

function getAbsoluteForce()
	return 0
end

function getPuissance()
	return 0
end

function getAbsolutePuissance()
	return 0
end

function getAgilite()
	return 0
end

function getAbsoluteAgilite()
	return 0
end

function getIntelligence()
	return 0
end

function getAbsoluteIntelligence()
	return 0
end

function getConstitution()
	return 0
end

function getAbsoluteConstitution()
	return 0
end

function getCharisme()
	return 0
end

function getAbsoluteCharisme()
	return 0
end

function getEsquive()
	return 0
end

function getAbsoluteEsquive()
	return 0
end

function getRecuperation()
	return 0
end

function getAbsoluteRecuperation()
	return 0
end

function getMultForce()
	if key == getIdEmplacement() then
		return multForce
	end
	return 0
end

function getAbsoluteMultForce()
	return multForce
end

function getMultPuissance()
	if key == getIdEmplacement() then
		return multPuissance
	end
	return 0
end

function getAbsoluteMultPuissance()
	return multPuissance
end

function getMultAgilite()
	if key == getIdEmplacement() then
		return multAgilite
	end
	return 0
end

function getAbsoluteMultAgilite()
	return multAgilite
end

function getMultIntelligence()
	if key == getIdEmplacement() then
		return multIntelligence
	end
	return 0
end

function getAbsoluteMultIntelligence()
	return multIntelligence
end

function getMultConstitution()
	return 0
end

function getAbsoluteMultConstitution()
	return 0
end

function getMultCharisme()
	return 0
end

function getAbsoluteMultCharisme()
	return 0
end

function getMultEsquive()
	return 0
end

function getAbsoluteMultEsquive()
	return 0
end

function getMultRecuperation()
	return 0
end

function getAbsoluteMultRecuperation()
	return 0
end

function getVitesseCourse()
	return 0
end

function getAbsoluteVitesseCourse()
	return 0
end

function getVitesseAttaque()
	return 0
end

function getAbsoluteVitesseAttaque()
	return 0
end

function getVitesseBlesse()
	return 0
end

function getAbsoluteVitesseBlesse()
	return 0
end

function getCumul()
	return cumul
end

function getQuantite()
	return quantite
end

function setQuantite(value)
	quantite = value
end

function generateRandomObject(quality)
	quantite = quantite * quality
end

function objectSave()
	return duree .. " " .. quantite
end

function objectRecoverState(data)
	_, _, duree, quantite = string.find(data, "(%d+) (%d+)")
end