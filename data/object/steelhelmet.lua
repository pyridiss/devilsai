--[[

Object "Steel Helmet" - Great Helm 3 (203.obj)

]]


-- global section
-- --------------

internalNumber = 203

categoryObject = "regulier"
typeObject     = "casque"
classObject    = "casque"

currentSlot = 0
slotForUse = "equipment-helmet"

properties = {
    ["intellect"]          = 0,
    ["constitution"]       = 10,
    ["charisma"]           = 1,
    ["dodge"]              = 0,
    ["constitutionAmplifier"] = 0,
    ["injurySpeed"]        = 1
}

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationIntelligenceProba     = 10
ameliorationConstitutionProba     = 60
ameliorationCharismeProba         = 10
ameliorationEsquiveProba          = 10
ameliorationVitesseBlesseProba    = 5
ameliorationMultConstitutionProba = 5

ameliorationIntelligenceQte     = 2
ameliorationConstitutionQte     = 1
ameliorationCharismeQte         = 2
ameliorationEsquiveQte          = 2
ameliorationVitesseBlesseQte    = 10
ameliorationMultConstitutionQte = 5


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
	return "steelhelmet"
end

function getImageFile()
    return "img/O203+.png"
end

function getIconFile()
    return "img/O203-.png"
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

		if Choix <= ameliorationIntelligenceProba then
			properties["intellect"] = properties["intellect"] + 1
			quality = quality - ameliorationIntelligenceQte
		else
			Choix = Choix - ameliorationIntelligenceProba

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

							if Choix <= ameliorationMultConstitutionProba then
								properties["constitutionAmplifier"] = properties["constitutionAmplifier"] + 5
								quality = quality - ameliorationMultConstitutionQte
							end
						end
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["intellect"] .. " " .. properties["constitution"] .. " " .. properties["charisma"] .. " " .. properties["dodge"] .. " " .. properties["injurySpeed"] .. " " .. properties["constitutionAmplifier"]
end

function objectRecoverState(data)
	_, _, properties["intellect"], properties["constitution"], properties["charisma"], properties["dodge"], properties["injurySpeed"] = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
end
