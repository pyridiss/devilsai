--[[

Object "Shield" - Shield 1 (101.obj)

]]


-- global section
-- --------------

internalNumber = 101

categoryObject = "regulier"
typeObject     = "bouclier"
classObject    = "bouclier"

currentSlot = 0
slotForUse = "equipment-shield"

properties = {
    ["constitution"]       = 2,
    ["charisma"]           = 0,
    ["dodge"]              = 1,
    ["injurySpeed"]        = 0
}

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationConstitutionProba   = 55
ameliorationCharismeProba       = 15
ameliorationEsquiveProba        = 25
ameliorationVitesseBlesseProba  = 5

ameliorationConstitutionQte   = 1
ameliorationCharismeQte       = 3
ameliorationEsquiveQte        = 3
ameliorationVitesseBlesseQte  = 10


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
	return "shield"
end

function getImageFile()
    return "img/O101+.png"
end

function getIconFile()
    return "img/O101-.png"
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
				else
					Choix = Choix - ameliorationEsquiveProba

					if Choix <= ameliorationVitesseBlesseProba then
						properties["injurySpeed"] = properties["injurySpeed"] + 1
						quality = quality - ameliorationVitesseBlesseQte
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["constitution"] .. " " .. properties["charisma"] .. " " .. properties["dodge"] .. " " .. properties["injurySpeed"]
end

function objectRecoverState(data)
	_, _, properties["constitution"], properties["charisma"], properties["dodge"], properties["injurySpeed"] = string.find(data, "(%d+) (%d+) (%d+) (%d+)")
end
