--[[

Object "Heal Potion" - (600.obj)

]]


-- global section
-- --------------

internalNumber = 600

categoryObject = "temporaire"
typeObject     = "potion"
classObject    = "potion"

currentSlot = 0
slotForUse = "equipment-potion"

properties = {
    ["recovery"]           = 100
}

duree    = 600
cumul    = true
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
	return "healpotion"
end

function getImageFile()
    return "img/O600+.png"
end

function getIconFile()
    return "img/O600-.png"
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
	quantite = quantite * quality
end

function objectSave()
	return duree .. " " .. quantite
end

function objectRecoverState(data)
	_, _, duree, quantite = string.find(data, "(%d+) (%d+)")
end
