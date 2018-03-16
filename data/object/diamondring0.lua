--[[

Object "Diamond Ring 0" - Diamond Ring 0

]]

name = "diamondring0"

categoryObject = "regulier"
classObject    = "bague"

imageFile = "img/O500+.png"
iconFile = "img/O500-.png"

currentSlot = 0
slotForUse = "equipment-ring"

properties = {
    ["power"]              = 5,
    ["intellect"]          = 1,
    ["charisma"]           = 3
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
