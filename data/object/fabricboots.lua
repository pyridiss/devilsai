--[[

Object "Fabric Boots" - Sabaton 2

]]

name = "fabricboots"

categoryObject = "regulier"
classObject    = "bottes"

imageFile = "img/O402+.png"
iconFile = "img/O402-.png"

currentSlot = 0
slotForUse = "equipment-boots"

properties = {
    ["agility"]            = 10,
    ["constitution"]       = 5,
    ["dodge"]              = 1,
    ["runSpeed"]           = 0
}

duree    = -1
quantite = 1
qualite  = 1
stackable = false

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationAgiliteProba        = 35
ameliorationConstitutionProba   = 35
ameliorationEsquiveProba        = 25
ameliorationVitesseCourseProba  = 5

ameliorationAgiliteQte        = 1
ameliorationConstitutionQte   = 1
ameliorationEsquiveQte        = 2
ameliorationVitesseCourseQte  = 10


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
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["agility"] .. " " .. properties["constitution"] .. " " .. properties["dodge"] .. " " .. properties["runSpeed"]
end

function objectRecoverState(data)
	_, _, properties["agility"], properties["constitution"], properties["dodge"], properties["runSpeed"] = string.find(data, "(%d+) (%d+) (%d+) (%d+)")
end
