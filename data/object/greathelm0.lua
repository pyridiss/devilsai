--[[

Object "Great Helm 0" - Great Helm 0 (200.obj)

]]


-- global section
-- --------------

internalNumber = 200

categoryObject = "regulier"
typeObject     = "casque"
classObject    = "casque"

currentSlot = 0
slotForUse = "equipment-helmet"

properties = {
    ["constitution"]       = 3,
    ["charisma"]           = 2
}

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
	return slotForUse
end

function getInternalNumber()
	return internalNumber
end

function getFileName()
	return "greathelm0"
end

function getImageFile()
    return "img/O200+.png"
end

function getIconFile()
    return "img/O200-.png"
end

function setKey(value)
	currentSlot = value
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

function getObjectProperty(key)
    if properties[key] == nil then
        return 0
    end
    return properties[key]
end

function getCurrentObjectEffect(key)
    if currentSlot == slotForUse then
        return getObjectProperty(key)
    end
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
