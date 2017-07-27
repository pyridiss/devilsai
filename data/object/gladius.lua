--[[

Object "Gladius" - Gladius 1 (1.obj)

]]


-- global section
-- --------------

internalNumber = 1

categoryObject = "regulier"
typeObject     = "arme"
classObject    = "epee"

currentSlot = 0
slotForUse = "equipment-weapon"

properties = {
    ["strength"]           = 5,
    ["power"]              = 0,
    ["agility"]            = 0,
    ["attackSpeed"]        = 0
}

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationForceProba          = 60
ameliorationPuissanceProba      = 20
ameliorationAgiliteProba        = 15
ameliorationVitesseAttaqueProba = 5

ameliorationForceQte          = 1
ameliorationPuissanceQte      = 2
ameliorationAgiliteQte        = 2
ameliorationVitesseAttaqueQte = 10


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
	return "gladius"
end

function getImageFile()
    return "img/O001+.png"
end

function getIconFile()
    return "img/O001-.png"
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

			if Choix <= ameliorationPuissanceProba then
				properties["power"] = properties["power"] + 1
				quality = quality - ameliorationPuissanceQte
			else
				Choix = Choix - ameliorationPuissanceProba

				if Choix <= ameliorationAgiliteProba then
					properties["agility"] = properties["agility"] + 1
					quality = quality - ameliorationAgiliteQte
				else
					Choix = Choix - ameliorationAgiliteProba

					if Choix <= ameliorationVitesseAttaqueProba then
						properties["attackSpeed"] = properties["attackSpeed"] + 1
						quality = quality - ameliorationVitesseAttaqueQte
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["strength"] .. " " .. properties["power"] .. " " .. properties["agility"] .. " " .. properties["attackSpeed"]
end

function objectRecoverState(data)
	_, _, properties["strength"], properties["power"], properties["agility"], properties["attackSpeed"] = string.find(data, "(%d+) (%d+) (%d+) (%d+)")
end
