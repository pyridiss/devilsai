
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


/** FONCTIONS DE LA CLASSE Individu_Commun **/

Individu_Commun::Individu_Commun() : Individu()
{
	Classe = NULL;
	Act = -1;
}

String32& Individu_Commun::Get_Nom()
{
	return Classe->Nom;
}

short Individu_Commun::Get_ChampVision()
{
	return Classe->ChampVision;
}

short Individu_Commun::Get_RayonInteraction()
{
	return Classe->RayonInteraction;
}

int Individu_Commun::Get_Experience()
{
	return Classe->Experience;
}

Caracteristiques* Individu_Commun::Get_Caracs()
{
	return &(Classe->Caracs);
}

Statistiques* Individu_Commun::Get_Stats()
{
	return &Stats;
}

Activite* Individu_Commun::Get_Activite(string act)
{
	return Classe->Get_Activite(act);
}

bool Individu_Commun::Set_Activite(string nv)
{
	//Cette redéfinition permet d'adopter un comportement particulier lors de la mort

	bool Resultat = Element_Mouvant::Set_Activite(nv);

	if (Get_Act() == MORT && Get_Num() == Get_Activite(MORT)->numberOfImages-2)
	{
		int key = CLEF_COFFRE;

		Cadavre *corpse = Partie.CarteCourante->AjouterCadavre(Liste, PosX, PosY);
        corpse->Set_Individu(Type, Classe->corpseImageKey);

		TypeClassement = CLASSEMENT_CADAVRE;
		Diplomatie = DIPLOM_NEUTRE;

		//First, we add equipment:
		for (mapObjects::iterator i = Classe->Caracs.objects.objects.begin() ; i != Classe->Caracs.objects.objects.end() ; ++i)
		{
			corpse->objects.addObject(getStringFromLUA(i->second, "getFileName"), intToString(key));
			++key;
		}

		//Then, we create inventory and add it:
		for (list<TemplateObject>::iterator obj = Classe->inventory.begin() ; obj != Classe->inventory.end() ; ++obj)
		{
			corpse->objects.addObject(obj->fileName, intToString(key), obj->quality);
			++key;
		}

		//Finally, we create stuff according to probabilities and add it:
		for (list<TemplateObject>::iterator obj = Classe->stuff.begin() ; obj != Classe->stuff.end() ; ++obj)
		{
			if (rand()%100 < obj->probability)
			{
				corpse->objects.addObject(obj->fileName, intToString(key), obj->quality);
				++key;
			}
		}

		//If no object has been put, we can remove the corpse faster
		if (corpse->objects.objects.empty())
			corpse->Duree = 2;

		IncrementNum();
	}

	return Resultat;
}

void Individu_Commun::Disp_Masks(float RefX, float RefY)
{
	CircleShape MasqueVision(Classe->ChampVision);
	MasqueVision.setPosition(Vector2f(Options.ScreenW/2 - (RefX - PosX) - Classe->ChampVision, Options.ScreenH/2 + 12 - (RefY - PosY) - Classe->ChampVision));
	MasqueVision.setFillColor(Color(225, 225, 255, 25));

	CircleShape MasqueCollision(RayonCollision);
	MasqueCollision.setPosition(Vector2f(Options.ScreenW/2 - (RefX - PosX) - RayonCollision, Options.ScreenH/2 + 12 - (RefY - PosY) - RayonCollision));
	MasqueCollision.setFillColor(Color(255, 255, 255, 100));

	CircleShape MasqueAttaque(Classe->RayonInteraction);
	MasqueAttaque.setPosition(Vector2f(Options.ScreenW/2 - (RefX - PosX) - Classe->RayonInteraction, Options.ScreenH/2 + 12 - (RefY - PosY) - Classe->RayonInteraction));
	MasqueAttaque.setFillColor(Color(255, 0, 0, 150));

	Jeu.App.draw(MasqueVision);
	Jeu.App.draw(MasqueAttaque);
	Jeu.App.draw(MasqueCollision);
}

bool Individu_Commun::angleFixed()
{
    return Classe->angleFixed;
}

/** FONCTIONS DE LA CLASSE Classe_Commune **/

Classe_Commune::Classe_Commune()
{
}

Classe_Commune::~Classe_Commune()
{
	Activites.clear();
}

void Classe_Commune::Ajouter_Activite(string Id)
{
	Activite act;
	Activites.insert(MapActivites::value_type(Id, act));
	MapActivites::iterator i = Activites.find(Id);
	i->second.Id = Id;
}

Activite* Classe_Commune::Get_Activite(string Id)
{
	MapActivites::iterator i = Activites.find(Id);
	if (i == Activites.end()) return NULL;
	return &i->second;
}

void Classe_Commune::Copie_Element(Individu_Commun *elem)
{
	elem->ActDefaut = ActDefaut;
	elem->Set_Activite(ActDefaut);
	elem->RayonCollision = RayonCollision;
	elem->Diplomatie = Diplomatie;
	(*(elem->Get_Stats()))["Recuperation"] = Caracs["RecuperationMoyenne"];
    elem->lifetime = lifetime;

    if (angleFixed) elem->angle = fixedAngle;
}

void Classe_Commune::setAngleFixed(double angle)
{
    angleFixed = true;
    fixedAngle = angle;
}
