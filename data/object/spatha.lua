--[[

Object "Spatha" - Gladius 2

]]

name = "spatha"

categoryObject = "regulier"
classObject    = "epee"

imageFile = "img/O002+.png"
iconFile = "img/O002-.png"

currentSlot = 0
slotForUse = "equipment-weapon"

properties = {
    ["strength"]           = 20,
    ["power"]              = 5,
    ["agility"]            = 2,
    ["charisma"]           = 0,
    ["attackSpeed"]        = 0
}

duree    = -1
quantite = 1
qualite  = 1
stackable = false

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationForceProba          = 50
ameliorationPuissanceProba      = 20
ameliorationAgiliteProba        = 15
ameliorationCharismeProba       = 10
ameliorationVitesseAttaqueProba = 5

ameliorationForceQte          = 1
ameliorationPuissanceQte      = 2
ameliorationAgiliteQte        = 2
ameliorationCharismeQte       = 2
ameliorationVitesseAttaqueQte = 10


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

					if Choix <= ameliorationCharismeProba then
						properties["charisma"] = properties["charisma"] + 1
						quality = quality - ameliorationCharismeQte
					else
						Choix = Choix - ameliorationCharismeProba

						if Choix <= ameliorationVitesseAttaqueProba then
							properties["attackSpeed"] = properties["attackSpeed"] + 1
							quality = quality - ameliorationVitesseAttaqueQte
						end
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["strength"] .. " " .. properties["power"] .. " " .. properties["agility"] .. " " .. properties["charisma"] .. " " .. properties["attackSpeed"]
end

function objectRecoverState(data)
	_, _, properties["strength"], properties["power"], properties["agility"], properties["charisma"], properties["attackSpeed"] = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+)")
end
