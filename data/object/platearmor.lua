--[[

Object "Plate armor" - Cuirass 2 (302.obj)

]]


-- global section
-- --------------

internalNumber = 302

categoryObject = "regulier"
typeObject     = "armure"
classObject    = "armure"

currentSlot = 0
slotForUse = "equipment-armor"

properties = {
    ["strength"]           = 0,
    ["constitution"]       = 15,
    ["charisma"]           = 2,
    ["dodge"]              = 0
}

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationForceProba          = 10
ameliorationConstitutionProba   = 70
ameliorationCharismeProba       = 10
ameliorationEsquiveProba        = 10

ameliorationForceQte          = 1
ameliorationConstitutionQte   = 1
ameliorationCharismeQte       = 2
ameliorationEsquiveQte        = 2


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
	return "platearmor"
end

function getImageFile()
    return "img/O302+.png"
end

function getIconFile()
    return "img/O302-.png"
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
