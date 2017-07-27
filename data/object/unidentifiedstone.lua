--[[

Object "Unidentified Stone" - (700.obj)

]]


-- global section
-- --------------

internalNumber = 700

categoryObject = "charme"
typeObject     = "pierre"
classObject    = "pierre"

currentSlot = 0
slotForUse = "none"

properties = {
    ["strength"]           = 0
}

duree    = -1
cumul    = false
quantite = 1

descriptionManuelle    = true
descriptionAutomatique = false


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
	return "unidentifiedstone"
end

function getImageFile()
    return "img/O700+.png"
end

function getIconFile()
    return "img/O700-.png"
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
