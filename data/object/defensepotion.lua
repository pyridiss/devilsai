--[[

Object "Defense Potion" - (602.obj)

]]


-- global section
-- --------------

internalNumber = 602

categoryObject = "temporaire"
typeObject     = "potion"
classObject    = "potion"

key = 0

multConstitution = 10
multCharisme     = 15
multEsquive      = 15

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
	return "defensepotion"
end

function getImageFile()
    return "img/O602+.png"
end

function getIconFile()
    return "img/O602-.png"
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

function getRecuperationMoyenne()
	return 0
end

function getAbsoluteRecuperationMoyenne()
	return 0
end

function getMultForce()
	return 0
end

function getAbsoluteMultForce()
	return 0
end

function getMultPuissance()
	return 0
end

function getAbsoluteMultPuissance()
	return 0
end

function getMultAgilite()
	return 0
end

function getAbsoluteMultAgilite()
	return 0
end

function getMultIntelligence()
	return 0
end

function getAbsoluteMultIntelligence()
	return 0
end

function getMultConstitution()
	if key == getIdEmplacement() then
		return multConstitution
	end
	return 0
end

function getAbsoluteMultConstitution()
	return multConstitution
end

function getMultCharisme()
	if key == getIdEmplacement() then
		return multCharisme
	end
	return 0
end

function getAbsoluteMultCharisme()
	return multCharisme
end

function getMultEsquive()
	if key == getIdEmplacement() then
		return multEsquive
	end
	return 0
end

function getAbsoluteMultEsquive()
	return multEsquive
end

function getMultRecuperationMoyenne()
	return 0
end

function getAbsoluteMultRecuperationMoyenne()
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
