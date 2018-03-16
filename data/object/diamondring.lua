--[[

Object "Diamond Ring" - Diamond Ring 1

]]

name = "diamondring"

categoryObject = "regulier"
classObject    = "bague"

imageFile = "img/O501+.png"
iconFile = "img/O501-.png"

currentSlot = 0
slotForUse = "equipment-ring"

properties = {
    ["power"]              = 2,
    ["intellect"]          = 1,
    ["charisma"]           = 3,
    ["healingPower"]       = 0
}

duree    = -1
quantite = 1
qualite  = 1
stackable = false

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationPuissanceProba      = 30
ameliorationIntelligenceProba   = 20
ameliorationCharismeProba       = 30
ameliorationRecuperationProba   = 20

ameliorationPuissanceQte      = 2
ameliorationIntelligenceQte   = 3
ameliorationCharismeQte       = 2
ameliorationRecuperationQte   = 3


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
