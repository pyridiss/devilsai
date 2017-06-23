
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

#include <cmath>

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

#include "imageManager/imageManager.h"
#include "imageManager/image.h"

/** FONCTIONS DE LA CLASSE Paysage **/

Paysage::Paysage() : Element_Carte()
{
	TypeClassement = CLASSEMENT_NORMAL;
}

Paysage::~Paysage()
{
}

int Paysage::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	return ETAT_NORMAL;
}

int Paysage::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	return COLL_PRIM;
}

void Paysage::calculateCollisionRadius()
{
	if (repeatX == 1 && repeatY == 1) return;
}

void Paysage::Disp(RenderTarget& target, float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

	if (repeatX == 1 && repeatY == 1)
	{
        imageManager::display(target, "paysage", Type, target.getSize().x/2 - (RefX - position().x), target.getSize().y/2 - (RefY - position().y), true);
	}
}

/** Class Door **/

Door::Door() : Element_Carte()
{
	TypeClassement = CLASSEMENT_NORMAL;
}
Door::~Door()
{
}

int Door::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	return ETAT_NORMAL;
}

int Door::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	if (deniedDiplomacy == elem->Diplomatie) return COLL_PRIM;

	return COLL_OK;
}

void Door::Disp(RenderTarget& target, float RefX, float RefY)
{
	return;
}


/** FONCTIONS DE LA CLASSE Classe_Paysage **/

void Classe_Paysage::Copie_Element(Paysage *elem)
{
    elem->size = size;
	elem->TypeClassement = TypeClassement;
	elem->Diplomatie = 0;
}


/** FONCTIONS DE LA CLASSE Paysage_Mouvant **/

Paysage_Mouvant::Paysage_Mouvant() : Element_Mouvant()
{
	Classe = NULL;
}
Paysage_Mouvant::~Paysage_Mouvant()
{
}

void Paysage_Mouvant::Activation()
{
	Set_Activite(ACTIVATION);
}

Activite* Paysage_Mouvant::Get_Activite(string act)
{
	return Classe->Get_Activite(act);
}

int Paysage_Mouvant::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	//Gestion du temps :
	Retour = Element_Mouvant::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	if (Classe->ActivationNecessaire) Set_Activite(STOP);
	else Set_Activite(MARCHE);

	IncrementNum();

	return ETAT_NORMAL;
}

void Paysage_Mouvant::IncrementNum(bool RaZ)
{
	if (RaZ)
	{
		Num = 0;
		ActEffectue = false;
	}
	else
	{
		++Num;
		if (Num == Get_Activite(Act)->numberOfImages) Num = 0;

		if (Num == 0 && Act == ACTIVATION) ActEffectue = true;
		else ActEffectue = false;
	}
}

int Paysage_Mouvant::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS) return COMPORTEMENT_ALEATOIRE;
	return COLL_PRIM;
}

void Paysage_Mouvant::Disp(RenderTarget& target, float RefX, float RefY)
{
	if (Controle == AI_IMG_HORSCHAMP) return;

    Activite* act = Get_Activite(Act);
    imageManager::display(target, "movingObjects", act->getImageKey(angle, Num), target.getSize().x/2 - (RefX - position().x), target.getSize().y/2 - (RefY - position().y), true);
}


/** FONCTIONS DE LA CLASSE Classe_Paysage_Mouvant **/

void Classe_Paysage_Mouvant::Copie_Element(Element_Carte *elem)
{
    elem->size = size;
	elem->TypeClassement = TypeClassement;
	elem->Diplomatie = 0;
}

void Classe_Paysage_Mouvant::Ajouter_Activite(string Id)
{
	Activite act;
	Activites.insert(MapActivites::value_type(Id, act));
	MapActivites::iterator i = Activites.find(Id);
	i->second.Id = Id;
}

Activite* Classe_Paysage_Mouvant::Get_Activite(string Id)
{
	MapActivites::iterator i = Activites.find(Id);
	if (i == Activites.end()) return NULL;
	return &i->second;
}
