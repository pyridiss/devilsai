
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
    inert = true;
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

void Paysage::Disp(RenderTarget& target)
{
	if (repeatX == 1 && repeatY == 1)
	{
        imageManager::display(target, "paysage", Type, position().x, position().y, true);
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

void Door::Disp(RenderTarget& target)
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
