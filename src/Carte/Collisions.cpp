
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
#include "../Jeu/Jeu.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Carte.h"


/** VARIABLES GLOBALES POUR LA GESTION DES COLLISIONS **/

list<Element_Carte*>::iterator currentCollider = Partie.colliders.end();
list<Element_Carte*>::iterator lastCollider = Partie.colliders.end();

int MaximumRayonCollision = 0;


/** OPTIMISATIONS DE LA LISTE DE COLLISION **/

void CalculerRayonMax(int i)
{
	if (i*1.1 > MaximumRayonCollision) MaximumRayonCollision = i*1.1;
}

bool comparisonBetweenColliders(Element_Carte* a, Element_Carte* b)
{
	if (a == NULL || b == NULL) return false;
	if (a->PosY < b->PosY) return true;

	return false;
}

void TriCollision()
{
	Partie.colliders.sort(comparisonBetweenColliders);
}


/** FONCTIONS DE DÉTECTION DES COLLISIONS **/

Element_Carte* Get_Current_Coll()
{
	if (lastCollider != Partie.colliders.end()) return *lastCollider;
	return NULL;
}

void RaZ_Coll()
{
	currentCollider = Partie.colliders.begin();
	lastCollider = Partie.colliders.begin();
}

bool Collision_cercle_cercle(int Ax, int Ay, int Ar, int Bx, int By, int Br)
{
	if (Ax + Ar < Bx - Br) return false;	if (Ax - Ar > Bx + Br) return false;
	if (Ay + Ar < By - Br) return false;	if (Ay - Ar > By + Br) return false;
	if (Ar == 0 || Br == 0) return false;
	if ((Ax-Bx)*(Ax-Bx) + (Ay-By)*(Ay-By) <= (Ar+Br)*(Ar+Br)) return true;
	return false;
}

bool Collision_cercle_rectangle(int Cx, int Cy, int Cr, int Rx, int Ry, int Rrx, int Rry)
{
	if (Cx + Cr < Rx - Rrx) return false;	if (Cx - Cr > Rx + Rrx) return false;
	if (Cy + Cr < Ry - Rry) return false;	if (Cy - Cr > Ry + Rry) return false;
	if (Cr == 0 || Rrx == 0 || Rry == 0) return false;
	if ((Cx-Rx)*(Cx-Rx) + (Cy-Ry)*(Cy-Ry) <= (Cr + Maximum(Rrx, Rry))*(Cr + Maximum(Rrx, Rry))) return true;
	return false;
}

bool Collision_rectangle_rectangle(int Ax, int Ay, int Arx, int Ary, int Bx, int By, int Brx, int Bry)
{
	if (Ax + Arx < Bx - Brx) return false;	if (Ax - Arx > Bx + Brx) return false;
	if (Ay + Ary < By - Bry) return false;	if (Ay - Ary > By + Bry) return false;
	return true;
}

int ParcoursCollisions(Individu *elem)
{
	//On suppose que cette fonction n'est appelée que par des Individus, PAS DE PAYSAGES

	int Retour = COLL_OK;

	if (currentCollider == Partie.colliders.end())
	{
		currentCollider = Partie.colliders.begin();
		return COLL_END;
	}

	static float elemPosY = 0;
	elemPosY = elem->PosY;

	//On avance dans la liste jusqu'à trouver un élément dont la différence des PosY est inférieure à MaximumRayonCollision
	while (currentCollider != Partie.colliders.end() && elemPosY - (*currentCollider)->PosY > MaximumRayonCollision)
	{
		++currentCollider;
	}

	//On vérifie que l'on ne dépasse plus MaximumRayonCollision, sinon le parcours est terminé
	if (currentCollider != Partie.colliders.end() && (*currentCollider)->PosY - elemPosY > MaximumRayonCollision)
	{
		currentCollider = Partie.colliders.begin();
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

	bool ResultColl = false;

	if (elem->ModeCollision == MODE_COLLISION_CERCLE && collider->ModeCollision == MODE_COLLISION_CERCLE)
		ResultColl = Collision_cercle_cercle(elem->PosX, elemPosY, elem->RayonCollision,
											 collider->PosX, collider->PosY, collider->RayonCollision);
	if (elem->ModeCollision == MODE_COLLISION_CERCLE && collider->ModeCollision == MODE_COLLISION_RECT)
		ResultColl = Collision_cercle_rectangle(elem->PosX, elemPosY, elem->RayonCollision,
												collider->PosX, collider->PosY, collider->RayX, collider->RayY);
	if (elem->ModeCollision == MODE_COLLISION_RECT && collider->ModeCollision == MODE_COLLISION_CERCLE)
		ResultColl = Collision_cercle_rectangle(collider->PosX, collider->PosY, collider->RayonCollision,
												elem->PosX, elemPosY, elem->RayX, elem->RayY);
	if (elem->ModeCollision == MODE_COLLISION_RECT && collider->ModeCollision == MODE_COLLISION_RECT)
		ResultColl = Collision_rectangle_rectangle(elem->PosX, elem->PosY, elem->RayX, elem->RayY,
												   collider->PosX, collider->PosY, collider->RayX, collider->RayY);

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

	//La suite teste les collisions en interaction et en vision : cela ne concerne pas les tmp paysages
	//Attention : seuls les paysages en RECT_COL sont bloqués ici ; un moyen pour les bloquer tous ?
	if (elem->Get_RayonInteraction() == 0 || elem->ModeCollision == MODE_COLLISION_RECT)
	{
		lastCollider = currentCollider;
		++currentCollider;
		return COLL_OK;
	}

	//Pas de collision primaire ; on teste une collision en interaction
	ResultColl = Collision_cercle_cercle(elem->PosX, elemPosY, elem->Get_RayonInteraction(),
										 collider->PosX, collider->PosY, collider->RayonCollision);
	if (ResultColl)
	{
		if (TestAngle(elem->PosX, elemPosY, elem->Get_Dir(), collider->PosX, collider->PosY, elem->Get_NombreDir()))
			Retour = COLL_INTER;
		else Retour = COLL_INTER_ARR;
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
	if (collider->Diplomatie != DIPLOM_NEUTRE && Retour == COLL_INTER_ARR) Retour = COLL_ATT_ARR;

	//Pas de collision primaire, pas de collision en interaction ; on teste une collision en vision
	if (Retour != COLL_INTER && Retour != COLL_INTER_ARR && Retour != COLL_ATT && Retour != COLL_ATT_ARR &&
		elem->Get_ChampVision() != 0)
	{ /** DONC, POUR LE MOMENT, Retour == COLL_OK **/
		ResultColl = Collision_cercle_cercle(elem->PosX, elemPosY, elem->Get_ChampVision(),
											 collider->PosX, collider->PosY, collider->RayonCollision);
		if (ResultColl) Retour = COLL_VIS;
	}

	lastCollider = currentCollider;
	++currentCollider;

	return Retour;
}
