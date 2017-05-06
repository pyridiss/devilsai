--[[

Object "Fabric Boots" - Sabaton 2 (402.obj)

]]


-- global section
-- --------------

internalNumber = 402

categoryObject = "regulier"
typeObject     = "bottes"
classObject    = "bottes"

key = 0

agilite       = 10
constitution  = 5
esquive       = 1
vitesseCourse = 0

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

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
	return "fabricboots"
end

function getImageFile()
    return "img/O402+.png"
end

function getIconFile()
    return "img/O402-.png"
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
	if key == getIdEmplacement() then
		return agilite
	end
	return 0
end

function getAbsoluteAgilite()
	return agilite
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
	return 0
end

function getAbsoluteCharisme()
	return 0
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
	if key == getIdEmplacement() then
		return vitesseCourse
	end
	return 0
end

function getAbsoluteVitesseCourse()
	return vitesseCourse
end

function getVitesseAttaque()
	return 0
end

function getAbsoluteVitesseAttaque()
	return 0
end

function getVitesseBlesse()
	return 0
end

function getAbsoluteVitesseBlesse()
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
			agilite = agilite + 1
			quality = quality - ameliorationAgiliteQte
		else
			Choix = Choix - ameliorationAgiliteProba

			if Choix <= ameliorationConstitutionProba then
				constitution = constitution + 1
				quality = quality - ameliorationConstitutionQte
			else
				Choix = Choix - ameliorationConstitutionProba

				if Choix <= ameliorationEsquiveProba then
					esquive = esquive + 1
					quality = quality - ameliorationEsquiveQte
				else
					Choix = Choix - ameliorationEsquiveProba

					if Choix <= ameliorationVitesseCourseProba then
						vitesseCourse = vitesseCourse + 1
						quality = quality - ameliorationVitesseCourseQte
					end
				end
			end
		end
	end
end

function objectSave()
	return agilite .. " " .. constitution .. " " .. esquive .. " " .. vitesseCourse
end

function objectRecoverState(data)
	_, _, agilite, constitution, esquive, vitesseCourse = string.find(data, "(%d+) (%d+) (%d+) (%d+)")
end
