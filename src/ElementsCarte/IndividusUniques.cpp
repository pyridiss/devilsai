
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

void Individu_Unique::Ajouter_Activite(int Id)
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

short Individu_Unique::Get_ChampVision()
{
	return ChampVision;
}

short Individu_Unique::Get_RayonInteraction()
{
	return RayonInteraction;
}

short Individu_Unique::Get_NombreDir()
{
	return Get_Activite(Act)->NombreDirections();
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

Activite* Individu_Unique::Get_Activite(short act)
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
		if (Get_Recuperation() > 0)
		{
			if (100*buf_rec <= 3*Get_Recuperation())
			{
				Lag_Vitalite(1);
				if (Get_Recuperation() > 80) Lag_Vitalite(3);
				if (Get_Recuperation() > 90) Lag_Vitalite(6);
				if (Get_Recuperation() > 95) Lag_Vitalite(6);
				Lag_Energie(1);
			}
		}
		if (Get_Recuperation() < 0)
		{
			if (100*buf_rec <= -3*Get_Recuperation())
			{
				Lag_Vitalite(-1);
				Lag_Energie(-1);
			}
		}
	}

	if (EnergieMax) Stats.Energie = 1000;

	//Diminue la durée de vie des objets utilisés -- Should maybe be placed in LUA scripts
	for (mapObjects::iterator i = Get_Caracs()->objects.objects.begin() ; i != Get_Caracs()->objects.objects.end() ; ++i)
	{
		if (getStringFromLUA(i->second, "getIdEmplacement") == i->first)
		{
			if (getDoubleFromLUA(i->second, "getDuree") > 0)
			{
				setDoubleToLUA(i->second, "setDuree", getDoubleFromLUA(i->second, "getDuree") - I(1));
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

unsigned int Individu_Unique::Get_Force()
{
	int Total = 1./2. * Get_Caracs()->Force * (1. + 1.2*Get_Vitalite()/1000.);

	pair<int, int> addedForce = Get_Caracs()->getFromObjectsAndSkills("Force");
	Total += addedForce.first;

	return Total + addedForce.second*Total/100.;
}

unsigned int Individu_Unique::Get_Puissance()
{
	int Total = 1./2. * Get_Caracs()->Puissance * (1. + 1.2*Get_Vitalite()/1000.);

	pair<int, int> addedPuissance = Get_Caracs()->getFromObjectsAndSkills("Puissance");
	Total += addedPuissance.first;

	return Total + addedPuissance.second*Total/100.;
}

unsigned short Individu_Unique::Get_Agilite()
{
	int Total = 1./2. * Get_Caracs()->Agilite * (1. + 1.2*Get_Vitalite()/1000.);

	pair<int, int> addedAgilite = Get_Caracs()->getFromObjectsAndSkills("Agilite");
	Total += addedAgilite.first;

	return Total + addedAgilite.second*Total/100.;
}

unsigned short Individu_Unique::Get_Intelligence()
{
	int Total = 1./2. * Get_Caracs()->Intelligence * (1. + 1.2*Get_Vitalite()/1000.);

	pair<int, int> addedIntelligence = Get_Caracs()->getFromObjectsAndSkills("Intelligence");
	Total += addedIntelligence.first;

	return Total + addedIntelligence.second*Total/100.;
}

unsigned short Individu_Unique::Get_Constitution()
{
	int Total = 1./2. * Get_Caracs()->Constitution * (1. + 1.2*Get_Vitalite()/1000.);

	pair<int, int> addedConstitution = Get_Caracs()->getFromObjectsAndSkills("Constitution");
	Total += addedConstitution.first;

	return Total + addedConstitution.second*Total/100.;
}

unsigned short Individu_Unique::Get_Esquive()
{
	int Total = 1./2. * Get_Caracs()->Esquive * (1. + 1.2*Get_Vitalite()/1000.);

	pair<int, int> addedEsquive = Get_Caracs()->getFromObjectsAndSkills("Esquive");
	Total += addedEsquive.first;

	return Total + addedEsquive.second*Total/100.;
}

unsigned short Individu_Unique::Get_Charisme()
{
	int Total = 1./2. * Get_Caracs()->Charisme * (1. + 1.2*Get_Vitalite()/1000.);

	pair<int, int> addedCharisme = Get_Caracs()->getFromObjectsAndSkills("Charisme");
	Total += addedCharisme.first;

	return Total + addedCharisme.second*Total/100.;
}

int Individu_Unique::Get_RecuperationMoyenne()
{
	int Total = Get_Caracs()->RecuperationMoyenne;

	pair<int, int> addedRecuperation = Get_Caracs()->getFromObjectsAndSkills("Recuperation");
	Total += addedRecuperation.first;

	return Total + addedRecuperation.second*Total/100.;
}

int Individu_Unique::Get_Vitesse(short act)
{
	int Total = Get_Activite(act)->Vitesse;

	//Workaround to make 'speedpotion' work. To be changed!

	string speedType = "";

	switch (act)
	{
		case 3 : speedType = "getVitesseCourse"; break;
		case 6 : speedType = "getVitesseBlesse"; break;
		case 101 :
		case 301 : speedType = "getVitesseAttaque"; break;
	}

	if (speedType != "")
		for (mapObjects::iterator i = Get_Caracs()->objects.objects.begin() ; i != Get_Caracs()->objects.objects.end() ; ++i)
		{
			Total += getIntFromLUA(i->second, speedType);
		}

	return Total;
}

bool Individu_Unique::Set_Activite(int nv)
{
	//Cette redéfinition permet d'adopter un comportement particulier lors de la mort

	bool Resultat = Element_Mouvant::Set_Activite(nv);

	if (Get_Act() == MORT)
	{
		Set_Vitalite(0);
		Set_Energie(0);
		Set_Recuperation(0);
	}
	if (Get_Act() == MORT && Get_Num() == Get_Activite(MORT)->Num_Max[Dir]-2)
	{
		int key = CLEF_COFFRE;

		Cadavre *corpse = Partie.CarteCourante->AjouterCadavre(Liste, PosX, PosY);
		corpse->Set_Individu(Type, Dir, Get_Activite(MORT)->Num_Max[Dir], true);
		Ajouter_ElementCollision(corpse);

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

void Individu_Unique::Disp_Masks(float RefX, float RefY)
{
	CircleShape MasqueVision(ChampVision);
	MasqueVision.setPosition(Vector2f(Options.ScreenW/2 - (RefX - PosX) - ChampVision, Options.ScreenH/2 + 12 - (RefY - PosY) - ChampVision));
	MasqueVision.setFillColor(Color(225, 225, 255, 25));

	CircleShape MasqueCollision(RayonCollision);
	MasqueCollision.setPosition(Vector2f(Options.ScreenW/2 - (RefX - PosX) - RayonCollision, Options.ScreenH/2 + 12 - (RefY - PosY) - RayonCollision));
	MasqueCollision.setFillColor(Color(255, 255, 255, 100));

	CircleShape MasqueAttaque(RayonInteraction);
	MasqueAttaque.setPosition(Vector2f(Options.ScreenW/2 - (RefX - PosX) - RayonInteraction, Options.ScreenH/2 + 12 - (RefY - PosY) - RayonInteraction));
	MasqueAttaque.setFillColor(Color(255, 0, 0, 150));

	Jeu.App.draw(MasqueVision);
	Jeu.App.draw(MasqueAttaque);
	Jeu.App.draw(MasqueCollision);
}
