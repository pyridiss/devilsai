--[[

Object "Shield 0" - Shield 0 (100.obj)

]]


-- global section
-- --------------

internalNumber = 100

categoryObject = "regulier"
typeObject     = "bouclier"
classObject    = "bouclier"

key = 0

constitution = 3
esquive      = 2

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
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
	return "shield0"
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
	if key == getIdEmplacement() then
		return constitution
	end
	return 0
end

function getAbsoluteConstitution()
	return constitution
end

function getCharisme()
	return 0
end

function getAbsoluteCharisme()
	return 0
end

function getEsquive()
	if key == getIdEmplacement() then
		return esquive
	end
	return 0
end

function getAbsoluteEsquive()
	return esquive
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

end

function objectSave()
	return "none"
end

function objectRecoverState(data)
	
end