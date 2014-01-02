--[[

Skill "Fire Ball"

]]


-- global section
-- --------------

owner = 0 -- This is a pointer to the owner of this skill

projectile = 0 -- Pointer to the created projectile

animation = 301

needsToBeActivated = true
activated          = false
activiteFinished   = false

projectile_id = 1001 -- Type
projectile_rect_x = 5
projectile_rect_y = 5
projectile_dir_0 = true
projectile_dir_1 = true
projectile_dir_2 = true
projectile_dir_3 = true
projectile_dir_4 = true
projectile_dir_5 = true
projectile_dir_6 = true
projectile_dir_7 = true
projectile_vitesse = 35
projectile_maj = 8
projectile_chp_att = 300
projectile_force = 140
projectile_puissance = 100
projectile_agilite = 180
projectile_intelligence = 5
projectile_cout_fixe = 5


-- functions
-- ---------

function skillBegin(_owner)
	owner = _owner
	addImageProjectile(projectile_id, 0);
	addImageProjectile(projectile_id, 1);
	addImageProjectile(projectile_id, 2);
	addImageProjectile(projectile_id, 3);
	addImageProjectile(projectile_id, 4);
	addImageProjectile(projectile_id, 5);
	addImageProjectile(projectile_id, 6);
	addImageProjectile(projectile_id, 7);
end

function setActivated(value)
	activated = value
end

function getActivated()
	return activated
end

function setActiviteFinished(value)
	activiteFinished = value
end

function use()
	if activated == true and activiteFinished == true and testEnoughEnergy() == true then

		projectile = createProjectile(projectile_id)
		set(projectile, "RayX", projectile_rect_x)
		set(projectile, "RayY", projectile_rect_y)
		set(projectile, "Num_Max_0", projectile_dir_0)
		set(projectile, "Num_Max_1", projectile_dir_1)
		set(projectile, "Num_Max_2", projectile_dir_2)
		set(projectile, "Num_Max_3", projectile_dir_3)
		set(projectile, "Num_Max_4", projectile_dir_4)
		set(projectile, "Num_Max_5", projectile_dir_5)
		set(projectile, "Num_Max_6", projectile_dir_6)
		set(projectile, "Num_Max_7", projectile_dir_7)

		set(projectile, "Vitesse", projectile_vitesse)
		set(projectile, "MaJ", projectile_maj)
		set(projectile, "ChampAttaque", projectile_chp_att)
		set(projectile, "Force", projectile_force)
		set(projectile, "Puissance", projectile_puissance)
		set(projectile, "Agilite", projectile_agilite)
		set(projectile, "Intelligence", projectile_intelligence)

		--On place le projectile à 20px du centre du joueur
		set(projectile, "Diplomatie", 1) -- DIPLOM_ALLIE
		set(projectile, "Controle", 1)   -- CONTROLE_HUMAIN
		set(projectile, "OrigineX", get(owner, "PosX") + dirToCoeff("X", get(owner, "Dir")) * 20)
		set(projectile, "OrigineY", get(owner, "PosY") + dirToCoeff("Y", get(owner, "Dir")) * 20)
		set(projectile, "Dir", get(owner, "Dir"))

		set(projectile, "PosX", get(owner, "PosX") + dirToCoeff("X", get(owner, "Dir")) * 20)
		set(projectile, "PosY", get(owner, "PosY") + dirToCoeff("Y", get(owner, "Dir")) * 20)

		set(owner, "energy", get(owner, "energy") - getNeededEnergy())
		activated = false
	end
	activiteFinished = false
end

function getActivite()
	return animation
end

function getInternalNumber()
	return animation
end

function getDegats()
	return degats
end

function getAmplitude()
	return amplitude
end

function getNeededEnergy()
	return projectile_puissance / get(owner, "Constitution") + projectile_cout_fixe
end

function testEnoughEnergy()

	energy = get(owner, "energy")

	if (energy < 2 * getNeededEnergy()) then
		return false
	else
		return true
	end

end

function getForce()
	return 0
end

function getPuissance()
	return 0
end

function getAgilite()
	return 0
end

function getIntelligence()
	return 0
end

function getConstitution()
	return 0
end

function getCharisme()
	return 0
end

function getEsquive()
	return 0
end

function getRecuperationMoyenne()
	return 0
end

function getMultForce()
	return 0
end

function getMultPuissance()
	return 0
end

function getMultAgilite()
	return 0
end

function getMultIntelligence()
	return 0
end

function getMultConstitution()
	return 0
end

function getMultCharisme()
	return 0
end

function getMultEsquive()
	return 0
end

function getMultRecuperationMoyenne()
	return 0
end

function getVitesseCourse()
	return 0
end

function getVitesseAttaque()
	return 0
end

function getVitesseBlesse()
	return 0
end

function dispImage(x, y)
	dispImageSkill(getInternalNumber(), x, y)
end