--[[

Object "Pavis" - Shield 2

]]

name = "pavis"

categoryObject = "regulier"
classObject    = "bouclier"

imageFile = "img/O102+.png"
iconFile = "img/O102-.png"

currentSlot = 0
slotForUse = "equipment-shield"

properties = {
    ["power"]              = 0,
    ["agility"]            = 0,
    ["constitution"]       = 8,
    ["charisma"]           = 0,
    ["dodge"]              = 4,
    ["injurySpeed"]        = 0
}

duree    = -1
quantite = 1
qualite  = 1
stackable = false

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationPuissanceProba      = 5
ameliorationAgiliteProba        = 5
ameliorationConstitutionProba   = 50
ameliorationCharismeProba       = 10
ameliorationEsquiveProba        = 25
ameliorationVitesseBlesseProba  = 5

ameliorationPuissanceQte      = 2
ameliorationAgiliteQte        = 2
ameliorationConstitutionQte   = 1
ameliorationCharismeQte       = 2
ameliorationEsquiveQte        = 2
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
	end
end

function objectSave()
	return properties["power"] .. " " .. properties["agility"] .. " " .. properties["constitution"] .. " " .. properties["charisma"] .. " " .. properties["dodge"] .. " " .. properties["injurySpeed"]
end

function objectRecoverState(data)
	_, _, properties["power"], properties["agility"], properties["constitution"], properties["charisma"], properties["dodge"], properties["injurySpeed"] = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
end
