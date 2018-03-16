--[[

Object "Unidentified Stone"

]]

name = "unidentifiedstone"

categoryObject = "charme"
classObject    = "pierre"

imageFile = "img/O700+.png"
iconFile = "img/O700-.png"

currentSlot = 0
slotForUse = "none"

properties = {
    ["strength"]           = 0
}

duree    = -1
quantite = 1
stackable = false

descriptionManuelle    = true
descriptionAutomatique = false


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
