--[[

Object "Attack Potion"

]]

name = "attackpotion"

categoryObject = "temporaire"
classObject    = "potion"

imageFile = "img/O601+.png"
iconFile = "img/O601-.png"

currentSlot = 0
slotForUse = "equipment-potion"

properties = {
    ["strengthAmplifier"]     = 10,
    ["powerAmplifier"]        = 10,
    ["agilityAmplifier"]      = 10,
    ["intellectAmplifier"]    = 5
}

duree    = 7200
quantite = 1

descriptionManuelle    = true
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

function stackable()
    if currentSlot == slotForUse then
        return false
    end
    return true
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
