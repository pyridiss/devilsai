--[[

Object "Steel Shield" - Shield 3 (103.obj)

]]


-- global section
-- --------------

internalNumber = 103

categoryObject = "regulier"
typeObject     = "bouclier"
classObject    = "bouclier"

currentSlot = 0
slotForUse = "equipment-shield"

properties = {
    ["agility"]            = 0,
    ["constitution"]       = 15,
    ["charisma"]           = 0,
    ["dodge"]              = 8,
    ["agilityAmplifier"]      = 0,
    ["injurySpeed"]        = 1
}

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationAgiliteProba        = 10
ameliorationConstitutionProba   = 35
ameliorationCharismeProba       = 20
ameliorationEsquiveProba        = 20
ameliorationVitesseBlesseProba  = 10
ameliorationMultAgiliteProba    = 5

ameliorationAgiliteQte        = 1
ameliorationConstitutionQte   = 1
ameliorationCharismeQte       = 2
ameliorationEsquiveQte        = 2
ameliorationVitesseBlesseQte  = 10
ameliorationMultAgiliteQte    = 5


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
	return "steelshield"
end

function getImageFile()
    return "img/O103+.png"
end

function getIconFile()
    return "img/O103-.png"
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

		if Choix <= ameliorationAgiliteProba then
			properties["agility"] = properties["agility"] + 1
			quality = quality - ameliorationAgiliteQte
		else
			Choix = Choix - ameliorationAgiliteProba

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
						else
							Choix = Choix - ameliorationVitesseBlesseProba

							if Choix <= ameliorationMultAgiliteProba then
								properties["agilityAmplifier"] = properties["agilityAmplifier"] + 5
								quality = quality - ameliorationMultAgiliteQte
							end
						end
					end
				end
			end
		end
	end
end


function objectSave()
	return properties["agility"] .. " " .. properties["constitution"] .. " " .. properties["charisma"] .. " " .. properties["dodge"] .. " " .. properties["injurySpeed"] .. " " .. properties["agilityAmplifier"]
end

function objectRecoverState(data)
	_, _, properties["agility"], properties["constitution"], properties["charisma"], properties["dodge"], properties["injurySpeed"] = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
end
