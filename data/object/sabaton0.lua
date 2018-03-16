--[[

Object "Sabaton 0" - Sabaton 0

]]

name = "sabaton0"

categoryObject = "regulier"
classObject    = "bottes"

imageFile = "img/O400+.png"
iconFile = "img/O400-.png"

currentSlot = 0
slotForUse = "equipment-boots"

properties = {
    ["agility"]            = 10,
    ["constitution"]       = 2
}

duree    = -1
quantite = 1
qualite  = 1
stackable = false

descriptionManuelle    = false
descriptionAutomatique = true


function active()
    return (currentSlot == slotForUse)
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

function generateRandomObject(quality)

end

function objectSave()
	return "none"
end

function objectRecoverState(data)
	
end
