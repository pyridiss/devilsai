--[[

Object "Cuirass" - Cuirass 1

]]

name = "cuirass"

categoryObject = "regulier"
classObject    = "armure"

imageFile = "img/O301+.png"
iconFile = "img/O301-.png"

currentSlot = 0
slotForUse = "equipment-armor"

properties = {
    ["strength"]           = 0,
    ["constitution"]       = 6,
    ["charisma"]           = 1,
    ["dodge"]              = 0
}

duree    = -1
quantite = 1
qualite  = 1
stackable = false

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationForceProba          = 10
ameliorationConstitutionProba   = 70
ameliorationCharismeProba       = 10
ameliorationEsquiveProba        = 10

ameliorationForceQte          = 2
ameliorationConstitutionQte   = 1
ameliorationCharismeQte       = 3
ameliorationEsquiveQte        = 3


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
	qualite = quality

	while quality > 0 do
		quality = quality - 1
		Choix = math.random()*100 + 1

		if Choix <= ameliorationForceProba then
			properties["strength"] = properties["strength"] + 1
			quality = quality - ameliorationForceQte
		else
			Choix = Choix - ameliorationForceProba

			if Choix <= ameliorationConstitutionProba then
				properties["constitution"] = properties["constitution"] + 1
				quality = quality - ameliorationConstitutionQte
			else
				Choix = Choix - ameliorationConstitutionProba

				if Choix <= ameliorationCharismeProba then
					properties["charisma"] = properties["charisma"] + 1
					quality = quality - ameliorationCharismeQte
				else
					Choix = Choix - ameliorationCharismeProba

					if Choix <= ameliorationEsquiveProba then
						properties["dodge"] = properties["dodge"] + 1
						quality = quality - ameliorationEsquiveQte
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["strength"] .. " " .. properties["constitution"] .. " " .. properties["charisma"] .. " " .. properties["dodge"]
end

function objectRecoverState(data)
	_, _, properties["strength"], properties["constitution"], properties["charisma"], properties["dodge"] = string.find(data, "(%d+) (%d+) (%d+) (%d+)")
end
