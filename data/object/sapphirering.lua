--[[

Object "Sapphire Ring" - Diamond Ring 2 (502.obj)

]]


-- global section
-- --------------

internalNumber = 502

categoryObject = "regulier"
typeObject     = "bague"
classObject    = "bague"

key = 0

puissance    = 5
intelligence = 2
charisme     = 5
recuperation = 0

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationPuissanceProba      = 30
ameliorationIntelligenceProba   = 20
ameliorationCharismeProba       = 30
ameliorationRecuperationProba   = 20

ameliorationPuissanceQte      = 1
ameliorationIntelligenceQte   = 3
ameliorationCharismeQte       = 2
ameliorationRecuperationQte   = 3


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
	return "sapphirering"
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
	return 0
end

function getAbsoluteConstitution()
	return 0
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
	return 0
end

function getAbsoluteEsquive()
	return 0
end

function getRecuperation()
	if key == getIdEmplacement() then
		return recuperation
	end
	return 0
end

function getAbsoluteRecuperation()
	return recuperation
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

function getMultRecuperation()
	return 0
end

function getAbsoluteMultRecuperation()
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

		if Choix <= ameliorationPuissanceProba then
			puissance = puissance + 1
			quality = quality - ameliorationPuissanceQte
		else
			Choix = Choix - ameliorationPuissanceProba

			if Choix <= ameliorationIntelligenceProba then
				intelligence = intelligence + 1
				quality = quality - ameliorationIntelligenceQte
			else
				Choix = Choix - ameliorationIntelligenceProba

				if Choix <= ameliorationCharismeProba then
					charisme = charisme + 1
					quality = quality - ameliorationCharismeQte
				else
					Choix = Choix - ameliorationCharismeProba

					if Choix <= ameliorationRecuperationProba then
						recuperation = recuperation + 1
						quality = quality - ameliorationRecuperationQte
					end
				end
			end
		end
	end
end

function objectSave()
	return puissance .. " " .. intelligence .. " " .. charisme .. " " .. recuperation
end

function objectRecoverState(data)
	_, _, puissance, intelligence, charisme, recuperation = string.find(data, "(%d+) (%d+) (%d+) (%d+)")
end