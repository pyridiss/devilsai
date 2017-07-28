--[[

Object "Speed Potion" - (603.obj)

]]


-- global section
-- --------------

internalNumber = 603

categoryObject = "temporaire"
typeObject     = "potion"
classObject    = "potion"

currentSlot = 0
slotForUse = "equipment-potion"

properties = {
    ["attackSpeed"]        = 3,
    ["runSpeed"]           = 3,
    ["injurySpeed"]        = 3
}

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
	return slotForUse
end

function getInternalNumber()
	return internalNumber
end

function getFileName()
	return "speedpotion"
end

function getImageFile()
    return "img/O603+.png"
end

function getIconFile()
    return "img/O603-.png"
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
    if currentSlot == slotForUse then
        return false
    end
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
