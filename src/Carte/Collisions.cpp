
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

#include "../Bibliotheque/Constantes.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Carte.h"


/** FONCTIONS DE DÉTECTION DES COLLISIONS **/

Element_Carte* Carte::getCurrentCollider()
{
	if (lastCollider != elements.end()) return *lastCollider;
	return NULL;
}

void Carte::resetCollisionManager()
{
	currentCollider = elements.begin();
	lastCollider = elements.begin();
}

int Carte::browseCollisionList(Individu *elem)
{
	//On suppose que cette fonction n'est appelée que par des Individus, PAS DE PAYSAGES

	int Retour = COLL_OK;

	static float elemPosX = 0, elemPosY = 0;
	elemPosX = elem->position().x; elemPosY = elem->position().y;

	const auto& end = elements.end();

    //If currentCollider has no collision, we can jump to the next one
    while (currentCollider != end && (*currentCollider)->ignoreCollision)
        ++currentCollider;

	if (currentCollider == end)
	{
		currentCollider = elements.begin();
		return COLL_END;
	}

	//Now we have our currentCollider; we can store the pointer to avoid use of operator*
	static Element_Carte* collider = NULL;
	collider = (*currentCollider);

	if (collider == elem)
	{
		lastCollider = currentCollider;
		++currentCollider;
		return COLL_OK;
	}

	bool ResultColl = tools::math::intersection(elem->size, collider->size);

	if (ResultColl)
	{
		//On a une collision entre les deux rayons primaires des éléments

		int Coll = collider->Collision(elem, COLL_PRIM);
		switch(Coll)
		{
			case COLL_OK		: break;
			case COLL_PRIM		: Retour = COLL_PRIM; break;
			case COLL_PRIM_MVT	: Retour = COLL_PRIM_MVT; break;
			case COLL_INTER		: Retour = COLL_INTER; break;
		}
	}

	//On a une collision primaire (ou interaction d'actionneur) ; Retour à la Gestion
	if (Retour == COLL_PRIM || Retour == COLL_PRIM_MVT || Retour == COLL_INTER)
	{
		lastCollider = currentCollider;
		++currentCollider;
		return Retour;
	}

    if (collider->inert)
    {
        lastCollider = currentCollider;
        ++currentCollider;
        return COLL_OK;
    }

	//Pas de collision primaire ; on teste une collision en interaction
    ResultColl = tools::math::intersection(elem->interactionField, collider->size);
	if (ResultColl)
	{
        Retour = COLL_INTER;
	}

	//Dans le cas de diplomatie identique, on s'arrête là
	if (elem->Diplomatie == collider->Diplomatie)
	{
		lastCollider = currentCollider;
		++currentCollider;
		return Retour;
	}

	//Dans le cas de la diplomatie adverse, on transforme l'interaction en attaque
	if (collider->Diplomatie != DIPLOM_NEUTRE && Retour == COLL_INTER) Retour = COLL_ATT;

	//Pas de collision primaire, pas de collision en interaction ; on teste une collision en vision
	if (Retour == COLL_OK)
	{
        ResultColl = tools::math::intersection(elem->viewField, collider->size);
		if (ResultColl) Retour = COLL_VIS;
	}

	lastCollider = currentCollider;
	++currentCollider;

	return Retour;
}
