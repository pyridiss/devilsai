--[[

Object "Cuirass 0" - Cuirass 0

]]

name = "cuirass0"

categoryObject = "regulier"
classObject    = "armure"

imageFile = "img/O300+.png"
iconFile = "img/O300-.png"

currentSlot = 0
slotForUse = "equipment-armor"

properties = {
    ["constitution"]       = 7,
    ["charisma"]           = 1
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
