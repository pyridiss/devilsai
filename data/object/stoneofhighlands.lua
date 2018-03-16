--[[

Object "Stone of Highlands" - Diamond Ring 3

]]

name = "stonehighlands"

categoryObject = "regulier"
classObject    = "bague"

imageFile = "img/O503+.png"
iconFile = "img/O503-.png"

currentSlot = 0
slotForUse = "equipment-ring"

properties = {
    ["power"]              = 8,
    ["intellect"]          = 4,
    ["charisma"]           = 5,
    ["healingPower"]       = 1,
    ["powerAmplifier"]        = 0,
    ["charismaAmplifier"]     = 0
}

duree    = -1
quantite = 1
qualite  = 1
stackable = false

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationPuissanceProba     = 25
ameliorationIntelligenceProba  = 20
ameliorationCharismeProba      = 25
ameliorationRecuperationProba  = 20
ameliorationMultPuissanceProba = 5
ameliorationMultCharismeProba  = 5

ameliorationPuissanceQte     = 1
ameliorationIntelligenceQte  = 2
ameliorationCharismeQte      = 2
ameliorationRecuperationQte  = 2
ameliorationMultPuissanceQte = 5
ameliorationMultCharismeQte  = 5


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
					else
						Choix = Choix - ameliorationRecuperationProba

						if Choix <= ameliorationMultPuissanceProba then
							properties["powerAmplifier"] = properties["powerAmplifier"] + 5
							quality = quality - ameliorationMultPuissanceQte
						else
							Choix = Choix - ameliorationMultPuissanceProba

							if Choix <= ameliorationMultCharismeProba then
								properties["charismaAmplifier"] = properties["charismaAmplifier"] + 5
								quality = quality - ameliorationMultCharismeQte
							end
						end
					end
				end
			end
		end
	end
end

function objectSave()
	return properties["power"] .. " " .. properties["intellect"] .. " " .. properties["charisma"] .. " " .. properties["healingPower"] .. " " .. properties["powerAmplifier"] .. " " .. properties["charismaAmplifier"]
end

function objectRecoverState(data)
	_, _, properties["power"], properties["intellect"], properties["charisma"], properties["healingPower"], properties["powerAmplifier"], properties["charismaAmplifier"] = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
end
