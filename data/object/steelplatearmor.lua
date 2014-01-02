--[[

Object "Steel plate armor" - Cuirass 3 (303.obj)

]]


-- global section
-- --------------

internalNumber = 303

categoryObject = "regulier"
typeObject     = "armure"
classObject    = "armure"

key = 0

force        = 0
constitution = 22
charisme     = 1
esquive      = 0

multForce        = 0
multConstitution = 0

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationAleatoire         = false

ameliorationForceProba            = 10
ameliorationConstitutionProba     = 60
ameliorationCharismeProba         = 10
ameliorationEsquiveProba          = 10
ameliorationMultForceProba        = 5
ameliorationMultConstitutionProba = 5

ameliorationForceQte            = 1
ameliorationConstitutionQte     = 1
ameliorationCharismeQte         = 2
ameliorationEsquiveQte          = 2
ameliorationMultForceQte        = 5
ameliorationMultConstitutionQte = 5


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
	return "steelplatearmor"
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
	if key == getIdEmplacement() then
		return force
	end
	return 0
end

function getAbsoluteForce()
	return force
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
	if key == getIdEmplacement() then
		return multForce
	end
	return 0
end

function getAbsoluteMultForce()
	return multForce
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

		if Choix <= ameliorationForceProba then
			force = force + 1
			quality = quality - ameliorationForceQte
		else
			Choix = Choix - ameliorationForceProba

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

						if Choix <= ameliorationMultForceProba then
							multForce = multForce + 5
							quality = quality - ameliorationMultForceQte
						else
							Choix = Choix - ameliorationMultForceProba

							if Choix <= ameliorationMultConstitutionProba then
								multConstitution = multConstitution + 5
								quality = quality - ameliorationMultConstitutionQte
							end
						end
					end
				end
			end
		end
	end
end

function objectSave()
	return force .. " " .. constitution .. " " .. charisme .. " " .. esquive .. " " .. multForce .. " " .. multConstitution
end

function objectRecoverState(data)
	_, _, force, constitution, charisme, esquive = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
end