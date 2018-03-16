--[[

Object "Shield 0" - Shield 0

]]

name = "shield0"

categoryObject = "regulier"
classObject    = "bouclier"

imageFile ="img/O100+.png"
iconFile = "img/O100-.png"

currentSlot = 0
slotForUse = "equipment-shield"

properties = {
    ["constitution"]       = 3,
    ["dodge"]              = 2
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
