--[[

Object "Steel Boots" - Sabaton 3 (403.obj)

]]


-- global section
-- --------------

internalNumber = 403

categoryObject = "regulier"
typeObject     = "bottes"
classObject    = "bottes"

currentSlot = 0
slotForUse = "equipment-boots"

properties = {
    ["power"]              = 5,
    ["agility"]            = 12,
    ["constitution"]       = 8,
    ["dodge"]              = 0,
    ["constitutionAmplifier"] = 0,
    ["dodgeAmplifier"]        = 0,
    ["runSpeed"]           = 1,
}

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationAgiliteProba          = 35
ameliorationConstitutionProba     = 20
ameliorationEsquiveProba          = 20
ameliorationVitesseCourseProba    = 15
ameliorationMultConstitutionProba = 5
ameliorationMultEsquiveProba      = 5

ameliorationAgiliteQte          = 1
ameliorationConstitutionQte     = 1
ameliorationEsquiveQte          = 1
ameliorationVitesseCourseQte    = 5
ameliorationMultConstitutionQte = 5
ameliorationMultEsquiveQte      = 5


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
	return "steelboots"
end

function getImageFile()
    return "img/O403+.png"
end

function getIconFile()
    return "img/O403-.png"
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

				if Choix <= ameliorationEsquiveProba then
					properties["dodge"] = properties["dodge"] + 1
					quality = quality - ameliorationEsquiveQte
				else
					Choix = Choix - ameliorationEsquiveProba

					if Choix <= ameliorationVitesseCourseProba then
						properties["runSpeed"] = properties["runSpeed"] + 1
						quality = quality - ameliorationVitesseCourseQte
					else
						Choix = Choix - ameliorationVitesseCourseProba

						if Choix <= ameliorationMultConstitutionProba then
							properties["constitutionAmplifier"] = properties["constitutionAmplifier"] + 5
							quality = quality - ameliorationMultConstitutionQte
						else
							Choix = Choix - ameliorationMultConstitutionProba

							if Choix <= ameliorationMultEsquiveProba then
								properties["dodgeAmplifier"] = properties["dodgeAmplifier"] + 5
								quality = quality - ameliorationMultEsquiveQte
							end
						end
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["power"] .. " " .. properties["agility"] .. " " .. properties["constitution"] .. " " .. properties["dodge"] .. " " .. properties["runSpeed"] .. " " .. properties["constitutionAmplifier"] .. " " .. properties["dodgeAmplifier"]
end

function objectRecoverState(data)
	_, _, properties["power"], properties["agility"], properties["constitution"], properties["dodge"], properties["runSpeed"], properties["constitutionAmplifier"], properties["dodgeAmplifier"] = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
end
