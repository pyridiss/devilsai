--[[

Object "Bascinet" - Great Helm 2 (202.obj)

]]


-- global section
-- --------------

internalNumber = 202

categoryObject = "regulier"
typeObject     = "casque"
classObject    = "casque"

key = 0

intelligence  = 0
constitution  = 7
charisme      = 2
esquive       = 0
vitesseBlesse = 0

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationIntelligenceProba   = 10
ameliorationConstitutionProba   = 65
ameliorationCharismeProba       = 10
ameliorationEsquiveProba        = 10
ameliorationVitesseBlesseProba  = 5

ameliorationIntelligenceQte   = 3
ameliorationConstitutionQte   = 1
ameliorationCharismeQte       = 2
ameliorationEsquiveQte        = 2
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
	return "bascinet"
end

function getImageFile()
    return "img/O202+.png"
end

function getIconFile()
    return "img/O202-.png"
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
	if key == getIdEmplacement() then
		return intelligence
	end
	return 0
end

function getAbsoluteIntelligence()
	return intelligence
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

		if Choix <= ameliorationIntelligenceProba then
			intelligence = intelligence + 1
			quality = quality - ameliorationIntelligenceQte
		else
			Choix = Choix - ameliorationIntelligenceProba

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
end

function objectSave()
	return intelligence .. " " .. constitution .. " " .. charisme .. " " .. esquive .. " " .. vitesseBlesse
end

function objectRecoverState(data)
	_, _, intelligence, constitution, charisme, esquive, vitesseBlesse = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+)")
end
