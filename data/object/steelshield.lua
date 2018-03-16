--[[

Object "Steel Shield" - Shield 3

]]

name = "steelshield"

categoryObject = "regulier"
classObject    = "bouclier"

imageFile = "img/O103+.png"
iconFile = "img/O103-.png"

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
quantite = 1
qualite  = 1
stackable = false

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
