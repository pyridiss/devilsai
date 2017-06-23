
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2014  Quentin Henriet

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tools/timeManager.h"

#include "../Bibliotheque/Constantes.h"
#include "../Carte/Carte.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"


/** FONCTIONS DE LA CLASSE Individu_Unique **/

Individu_Unique::Individu_Unique() : Individu()
{
	Act = -1;
}

Individu_Unique::~Individu_Unique()
{
// 	Equips.clear();
// 	for (int c = 0 ; c < NOMBRE_COMPETENCES ; ++c)
// 		CompetencesRapides[c] = NULL;
	Activites.clear();
// 	for (MapPointeursCompetences::iterator i = Competences.begin() ; i != Competences.end() ; ++i)
// 		delete i->second;
}

void Individu_Unique::Ajouter_Activite(string Id)
{
	Activite act;
	Activites.insert(MapActivites::value_type(Id, act));
	MapActivites::iterator i = Activites.find(Id);
	i->second.Id = Id;
}

void Individu_Unique::Ajouter_EmplacementEquip(int x, int y, int w, int h, string Categorie, string Type, string Classe)
{
	EmplacementEquipement emp;
	EmplacementsEquip.push_back(emp);
	EmplacementsEquip.back().Set(x, y, w, h);
	EmplacementsEquip.back().CategorieObjet = Categorie;
	EmplacementsEquip.back().TypeObjet = Type;
	EmplacementsEquip.back().ClasseObjet = Classe;
}

String32& Individu_Unique::Get_Nom()
{
	return Nom;
}

int Individu_Unique::Get_Experience()
{
	return Experience;
}

Caracteristiques* Individu_Unique::Get_Caracs()
{
	return &Caracs;
}

Statistiques* Individu_Unique::Get_Stats()
{
	return &Stats;
}

Activite* Individu_Unique::Get_Activite(string act)
{
	MapActivites::iterator i = Activites.find(act);
	if (i == Activites.end()) return NULL;
	return &i->second;
}

void Individu_Unique::Gestion_Recuperation()
{
	//Cette fonction s'occupe de la récupération, de l'énergie, et de la durée de vie des objets
	if (!RecuperationFixe) Individu::Gestion_Recuperation();
	else
	{
		if (get("Recuperation") > 0)
		{
			if (100*buf_rec <= 3*get("Recuperation"))
			{
				Lag_Vitalite(1);
				if (get("Recuperation") > 80) Lag_Vitalite(3);
				if (get("Recuperation") > 90) Lag_Vitalite(6);
				if (get("Recuperation") > 95) Lag_Vitalite(6);
				Lag_Energie(1);
			}
		}
		if (get("Recuperation") < 0)
		{
			if (100*buf_rec <= -3*get("Recuperation"))
			{
				Lag_Vitalite(-1);
				Lag_Energie(-1);
			}
		}
	}

	if (EnergieMax) Stats["Energie"] = 1000;

	//Diminue la durée de vie des objets utilisés -- Should maybe be placed in LUA scripts
	for (mapObjects::iterator i = Get_Caracs()->objects.objects.begin() ; i != Get_Caracs()->objects.objects.end() ; ++i)
	{
		if (getStringFromLUA(i->second, "getIdEmplacement") == i->first)
		{
			if (getDoubleFromLUA(i->second, "getDuree") > 0)
			{
                setDoubleToLUA(i->second, "setDuree", getDoubleFromLUA(i->second, "getDuree") - tools::timeManager::I(1));
				if (getDoubleFromLUA(i->second, "getDuree") <= 0)
				{
					lua_State *j = i->second;
					i = Get_Caracs()->objects.objects.erase(i);
					Get_Caracs()->objects.deleteObject(j);
					continue;
				}
			}
		}
	}
}

void Individu_Unique::Lag_Recuperation(float lag)
{
	if (!RecuperationFixe) Individu::Lag_Recuperation(lag);
}

float Individu_Unique::get(string field)
{
	float& valueFloat = (*Get_Stats())[field];
	if (valueFloat != Jeu.floatNotFound)
		return valueFloat;

	int valueInt = (*Get_Caracs())[field];
	if (valueInt != Jeu.intNotFound)
	{
		if (field != "RecuperationMoyenne") valueInt *= 1./2. * (1. + 1.2*get("Vitalite")/1000.);

		pair<int, int> addedChar = Get_Caracs()->getFromObjectsAndSkills(field);
		valueInt += addedChar.first;

		return valueInt + addedChar.second*valueInt/100.;
	}

	return Jeu.floatNotFound;
}

int Individu_Unique::Get_Vitesse(string act)
{
	int Total = Get_Activite(act)->speed;

	//Workaround to make 'speedpotion' work. To be changed!

	string speedType = "";

    if (act == "3") speedType = "getVitesseCourse";
    else if (act == "6") speedType = "getVitesseBlesse";
    else if (act == "101" || act == "301") speedType = "getVitesseAttaque";

	if (speedType != "")
	{
		for (auto& i : Get_Caracs()->objects.objects)
			Total += getIntFromLUA(i.second, speedType);

		for (auto& i : Get_Caracs()->skills)
			Total += getIntFromLUA(i.second, speedType);
	}

	return Total;
}

bool Individu_Unique::Set_Activite(string nv)
{
	//Cette redéfinition permet d'adopter un comportement particulier lors de la mort

	bool Resultat = Element_Mouvant::Set_Activite(nv);

	if (Get_Act() == MORT)
	{
		Set_Vitalite(0);
		Set_Energie(0);
		Set_Recuperation(0);
	}
	if (Get_Act() == MORT && Get_Num() == Get_Activite(MORT)->numberOfImages-2)
	{
		int key = CLEF_COFFRE;

		Cadavre *corpse = Partie.CarteCourante->AjouterCadavre(Liste, position().x, position().y);
        corpse->Set_Individu(Type, corpseImageKey);

		TypeClassement = CLASSEMENT_CADAVRE;
		Diplomatie = DIPLOM_NEUTRE;

		for (mapObjects::iterator i = Get_Caracs()->objects.objects.begin() ; i != Get_Caracs()->objects.objects.end() ; ++i)
		{
			corpse->objects.addObject(getStringFromLUA(i->second, "getFileName"), intToString(key));
			++key;
		}

		//If no object has been put, we can remove the corpse faster
		if (corpse->objects.objects.empty())
			corpse->Duree = 2;

		IncrementNum();
	}

	return Resultat;
}

/* La fonction Individu_Unique::Load() est développée dans le fichier ChargementElements.cpp */

bool Individu_Unique::angleFixed()
{
    return false;
}
