--[[

Object "Steel Boots" - Sabaton 3 (403.obj)

]]


-- global section
-- --------------

internalNumber = 403

categoryObject = "regulier"
typeObject     = "bottes"
classObject    = "bottes"

key = 0

puissance     = 5
agilite       = 12
constitution  = 8
esquive       = 0
vitesseCourse = 1

multConstitution = 0
multEsquive      = 0

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationAgiliteProba          = 35
ameliorationConstitutionProba     = 20
ameliorationEsquiveProba          = 20
ameliorationVitesseCourseProba    = 15
ameliorationMultConstitutionProba = 5
ameliorationMultEsquiveProba      = 5

ameliorationAgiliteQte          = 1
ameliorationConstitutionQte     = 1
ameliorationEsquiveQte          = 1
ameliorationVitesseCourseQte    = 5
ameliorationMultConstitutionQte = 5
ameliorationMultEsquiveQte      = 5


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
	return "steelboots"
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
	if key == getIdEmplacement() then
		return puissance
	end
	return 0
end

function getAbsolutePuissance()
	return puissance
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

function getRecuperation()
	return 0
end

function getAbsoluteRecuperation()
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
	if key == getIdEmplacement() then
		return multConstitution
	end
	return 0
end

function getAbsoluteMultConstitution()
	return multConstitution
end

function getMultCharisme()
	return 0
end

function getAbsoluteMultCharisme()
	return 0
end

function getMultEsquive()
	if key == getIdEmplacement() then
		return multEsquive
	end
	return 0
end

function getAbsoluteMultEsquive()
	return multEsquive
end

function getMultRecuperation()
	return 0
end

function getAbsoluteMultRecuperation()
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
					else
						Choix = Choix - ameliorationVitesseCourseProba

						if Choix <= ameliorationMultConstitutionProba then
							multConstitution = multConstitution + 5
							quality = quality - ameliorationMultConstitutionQte
						else
							Choix = Choix - ameliorationMultConstitutionProba

							if Choix <= ameliorationMultEsquiveProba then
								multEsquive = multEsquive + 5
								quality = quality - ameliorationMultEsquiveQte
							end
						end
					end
				end
			end
		end
	end
end

function objectSave()
	return puissance .. " " .. agilite .. " " .. constitution .. " " .. esquive .. " " .. vitesseCourse .. " " .. multConstitution .. " " .. multEsquive
end

function objectRecoverState(data)
	_, _, puissance, agilite, constitution, esquive, vitesseCourse, multConstitution, multEsquive = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
end