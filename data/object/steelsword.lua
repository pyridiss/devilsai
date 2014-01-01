--[[

Object "Steel Sword" - Gladius 3 (3.obj)

]]


-- global section
-- --------------

internalNumber = 3

categoryObject = "regulier"
typeObject     = "arme"
classObject    = "epee"

key = 0

force          = 40
puissance      = 7
agilite        = 2
charisme       = 1
vitesseAttaque = 0

multForce = 0

duree    = -1
cumul    = false
quantite = 1
qualite  = 1

descriptionManuelle    = false
descriptionAutomatique = true

ameliorationForceProba          = 40
ameliorationPuissanceProba      = 20
ameliorationAgiliteProba        = 15
ameliorationCharismeProba       = 10
ameliorationVitesseAttaqueProba = 10
ameliorationMultForceProba      = 5

ameliorationForceQte          = 1
ameliorationPuissanceQte      = 2
ameliorationAgiliteQte        = 2
ameliorationCharismeQte       = 2
ameliorationVitesseAttaqueQte = 10
ameliorationMultForceQte      = 5


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
	return "steelsword"
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
	return 0
end

function getAbsoluteRecuperation()
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
	if key == getIdEmplacement() then
		return vitesseAttaque
	end
	return 0
end

function getAbsoluteVitesseAttaque()
	return vitesseAttaque
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

			if Choix <= ameliorationPuissanceProba then
				puissance = puissance + 1
				quality = quality - ameliorationPuissanceQte
			else
				Choix = Choix - ameliorationPuissanceProba

				if Choix <= ameliorationAgiliteProba then
					agilite = agilite + 1
					quality = quality - ameliorationAgiliteQte
				else
					Choix = Choix - ameliorationAgiliteProba

					if Choix <= ameliorationCharismeProba then
						charisme = charisme + 1
						quality = quality - ameliorationCharismeQte
					else
						Choix = Choix - ameliorationCharismeProba

						if Choix <= ameliorationVitesseAttaqueProba then
							vitesseAttaque = vitesseAttaque + 1
							quality = quality - ameliorationVitesseAttaqueQte
						else
							Choix = Choix - ameliorationVitesseAttaqueProba

							if Choix <= ameliorationMultForceProba then
								multForce = multForce + 5
								quality = quality - ameliorationMultForceQte
							end
						end
					end
				end
			end
		end
	end
end

function objectSave()
	return force .. " " .. puissance .. " " .. agilite .. " " .. charisme .. " " .. vitesseAttaque .. " " .. multForce
end

function objectRecoverState(data)
	_, _, force, puissance, agilite, charisme, vitesseAttaque, multForce = string.find(data, "(%d+) (%d+) (%d+) (%d+) (%d+) (%d+) (%d+)")
end