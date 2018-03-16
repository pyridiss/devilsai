--[[

Object "Shield" - Shield 1

]]

name = "shield"

categoryObject = "regulier"
classObject    = "bouclier"

imageFile = "img/O101+.png"
iconFile = "img/O101-.png"

currentSlot = 0
slotForUse = "equipment-shield"

properties = {
    ["constitution"]       = 2,
    ["charisma"]           = 0,
    ["dodge"]              = 1,
    ["injurySpeed"]        = 0
}

duree    = -1
quantite = 1
qualite  = 1
stackable = false

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
