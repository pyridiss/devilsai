--[[

Object "Shield" - Shield 1 (101.obj)

]]


-- global section
-- --------------

internalNumber = 101

categoryObject = "regulier"
typeObject     = "bouclier"
classObject    = "bouclier"

key = 0

constitution  = 2
charisme      = 0
esquive       = 1
vitesseBlesse = 0

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

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


-- functions
-- ---------

function getCategorieObjet()
	return categoryObject
end

function getTypeObject()
	return typeObject
end

function getIdEmplacement()
	return categoryObject .. "-" .. typeObject
end

function getInternalNumber()
	return internalNumber
end

function getFileName()
	return "shield"
end

function getImageFile()
    return "img/O101+.png"
end

function getIconFile()
    return "img/O101-.png"
end

function setKey(value)
	key = value
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

function getForce()
	return 0
end

function getAbsoluteForce()
	return 0
end

function getPuissance()
	return 0
end

function getAbsolutePuissance()
	return 0
end

function getAgilite()
	return 0
end

function getAbsoluteAgilite()
	return 0
end

function getIntelligence()
	return 0
end

function getAbsoluteIntelligence()
	return 0
end

function getConstitution()
	if key == getIdEmplacement() then
		return constitution
	end
	return 0
end

function getAbsoluteConstitution()
	return constitution
end

function getCharisme()
	if key == getIdEmplacement() then
		return charisme
	end
	return 0
end

function getAbsoluteCharisme()
	return charisme
end

function getEsquive()
	if key == getIdEmplacement() then
		return esquive
	end
	return 0
end

function getAbsoluteEsquive()
	return esquive
end

function getRecuperationMoyenne()
	return 0
end

function getAbsoluteRecuperationMoyenne()
	return 0
end

function getMultForce()
	return 0
end

function getAbsoluteMultForce()
	return 0
end

function getMultPuissance()
	return 0
end

function getAbsoluteMultPuissance()
	return 0
end

function getMultAgilite()
	return 0
end

function getAbsoluteMultAgilite()
	return 0
end

function getMultIntelligence()
	return 0
end

function getAbsoluteMultIntelligence()
	return 0
end

function getMultConstitution()
	return 0
end

function getAbsoluteMultConstitution()
	return 0
end

function getMultCharisme()
	return 0
end

function getAbsoluteMultCharisme()
	return 0
end

function getMultEsquive()
	return 0
end

function getAbsoluteMultEsquive()
	return 0
end

function getMultRecuperationMoyenne()
	return 0
end

function getAbsoluteMultRecuperationMoyenne()
	return 0
end

function getVitesseCourse()
	return 0
end

function getAbsoluteVitesseCourse()
	return 0
end

function getVitesseAttaque()
	return 0
end

function getAbsoluteVitesseAttaque()
	return 0
end

function getVitesseBlesse()
	if key == getIdEmplacement() then
		return vitesseBlesse
	end
	return 0
end

function getAbsoluteVitesseBlesse()
	return vitesseBlesse
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

		if Choix <= ameliorationConstitutionProba then
			constitution = constitution + 1
			quality = quality - ameliorationConstitutionQte
		else
			Choix = Choix - ameliorationConstitutionProba

			if Choix <= ameliorationCharismeProba then
				charisme = charisme + 1
				quality = quality - ameliorationCharismeQte
			else
				Choix = Choix - ameliorationCharismeProba

				if Choix <= ameliorationEsquiveProba then
					esquive = esquive + 1
					quality = quality - ameliorationEsquiveQte
				else
					Choix = Choix - ameliorationEsquiveProba

					if Choix <= ameliorationVitesseBlesseProba then
						vitesseBlesse = vitesseBlesse + 1
						quality = quality - ameliorationVitesseBlesseQte
					end
				end
			end
		end
	end
end

function objectSave()
	return constitution .. " " .. charisme .. " " .. esquive .. " " .. vitesseBlesse
end

function objectRecoverState(data)
	_, _, constitution, charisme, esquive, vitesseBlesse = string.find(data, "(%d+) (%d+) (%d+) (%d+)")
end
