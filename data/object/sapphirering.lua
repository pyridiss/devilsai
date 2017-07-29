--[[

Object "Sapphire Ring" - Diamond Ring 2 (502.obj)

]]


-- global section
-- --------------

internalNumber = 502

categoryObject = "regulier"
typeObject     = "bague"
classObject    = "bague"

currentSlot = 0
slotForUse = "equipment-ring"

properties = {
    ["power"]              = 5,
    ["intellect"]          = 2,
    ["charisma"]           = 5,
    ["healingPower"]       = 0
}

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationPuissanceProba      = 30
ameliorationIntelligenceProba   = 20
ameliorationCharismeProba       = 30
ameliorationRecuperationProba   = 20

ameliorationPuissanceQte      = 1
ameliorationIntelligenceQte   = 3
ameliorationCharismeQte       = 2
ameliorationRecuperationQte   = 3


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
	return "sapphirering"
end

function getImageFile()
    return "img/O502+.png"
end

function getIconFile()
    return "img/O502-.png"
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

		if Choix <= ameliorationPuissanceProba then
			properties["power"] = properties["power"] + 1
			quality = quality - ameliorationPuissanceQte
		else
			Choix = Choix - ameliorationPuissanceProba

			if Choix <= ameliorationIntelligenceProba then
				properties["intellect"] = properties["intellect"] + 1
				quality = quality - ameliorationIntelligenceQte
			else
				Choix = Choix - ameliorationIntelligenceProba

				if Choix <= ameliorationCharismeProba then
					properties["charisma"] = properties["charisma"] + 1
					quality = quality - ameliorationCharismeQte
				else
					Choix = Choix - ameliorationCharismeProba

					if Choix <= ameliorationRecuperationProba then
						properties["healingPower"] = properties["healingPower"] + 1
						quality = quality - ameliorationRecuperationQte
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["power"] .. " " .. properties["intellect"] .. " " .. properties["charisma"] .. " " .. properties["healingPower"]
end

function objectRecoverState(data)
	_, _, properties["power"], properties["intellect"], properties["charisma"], properties["healingPower"] = string.find(data, "(%d+) (%d+) (%d+) (%d+)")
end
