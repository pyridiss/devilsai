--[[

Object "Defense Potion"

]]

name = "defensepotion"

categoryObject = "temporaire"
classObject    = "potion"

imageFile = "img/O602+.png"
iconFile = "img/O602-.png"

currentSlot = 0
slotForUse = "equipment-potion"

properties = {
    ["constitutionAmplifier"] = 10,
    ["charismaAmplifier"]     = 15,
    ["dodgeAmplifier"]        = 15
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
