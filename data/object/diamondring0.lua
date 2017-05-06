--[[

Object "Diamond Ring 0" - Diamond Ring 0 (500.obj)

]]


-- global section
-- --------------

internalNumber = 500

categoryObject = "regulier"
typeObject     = "bague"
classObject    = "bague"

key = 0

puissance    = 5
intelligence = 1
charisme     = 3

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
	return "diamondring0"
end

function getImageFile()
    return "img/O500+.png"
end

function getIconFile()
    return "img/O500-.png"
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
	if key == getIdEmplacement() then
		return puissance
	end
	return 0
end

function getAbsolutePuissance()
	return puissance
end

function getAgilite()
	return 0
end

function getAbsoluteAgilite()
	return 0
end

function getIntelligence()
	if key == getIdEmplacement() then
		return intelligence
	end
	return 0
end

function getAbsoluteIntelligence()
	return intelligence
end

function getConstitution()
	return 0
end

function getAbsoluteConstitution()
	return 0
end

function getCharisme()
	if key == getIdEmplacement() then
		return charisme
	end
	return 0
end

function getAbsoluteCharisme()
	return charisme
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
