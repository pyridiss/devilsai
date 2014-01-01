
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

Element_Collision *Coll_tmp = Partie.Collision_head;
Element_Collision *Coll_last = Coll_tmp;

int MaximumRayonCollision = 0;


/** FONCTIONS DE LA CLASSE Element_Collision **/

Element_Collision::Element_Collision()
{
	elem = NULL;
	next = NULL;
}

Element_Collision::~Element_Collision()
{
	if (next != NULL) delete next;
	next = NULL; elem = NULL;
}


/** OPTIMISATIONS DE LA LISTE DE COLLISION **/

void CalculerRayonMax(int i)
{
	if (i*1.1 > MaximumRayonCollision) MaximumRayonCollision = i*1.1;
}

void TriCollision()
{
	Element_Collision *Tri_tmp = Partie.Collision_head;

	if (Tri_tmp == NULL || Tri_tmp->next == NULL) return;

	if (Tri_tmp->elem->PosY > Tri_tmp->next->elem->PosY)
	{
		Element_Collision *tmp2 = Tri_tmp->next;
		Tri_tmp->next = tmp2->next;
		tmp2->next = Tri_tmp;
		Partie.Collision_head = tmp2;
	}

	while (Tri_tmp != NULL)
	{
		//Tests pour tenter de garder la liste dans l'ordre des y décroissants

			//Échange de Tri_tmp->next->next et de Tri_tmp->next->next->next

		if (Tri_tmp->next != NULL && Tri_tmp->next->next != NULL && Tri_tmp->next->next->next != NULL 
			&& Tri_tmp->next->next->elem->PosY > Tri_tmp->next->next->next->elem->PosY)
		{
			Element_Collision *tmp2 = Tri_tmp->next->next;
			Tri_tmp->next->next = tmp2->next;
			tmp2->next = tmp2->next->next;
			Tri_tmp->next->next->next = tmp2;
		}

			 //Échange de Tri_tmp->next et de Tri_tmp->next->next

		if (Tri_tmp->next != NULL && Tri_tmp->next->next != NULL && Tri_tmp->next->elem->PosY > Tri_tmp->next->next->elem->PosY)
		{
			Element_Collision *tmp2 = Tri_tmp->next;
			Tri_tmp->next = tmp2->next;
			tmp2->next = tmp2->next->next;
			Tri_tmp->next->next = tmp2;
		}

			//Remise en fin de chaîne de last

		while (Partie.Collision_last->next != NULL) Partie.Collision_last = Partie.Collision_last->next;

		Tri_tmp = Tri_tmp->next;
	}
}


/** FONCTIONS DE DÉTECTION DES COLLISIONS **/

Element_Carte* Get_Current_Coll()
{
	if (Coll_last != NULL) return Coll_last->elem;
	return NULL;
}

void RaZ_Coll()
{
	Coll_tmp = Partie.Collision_head;
	Coll_last = Partie.Collision_head;
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

	if (Coll_tmp == NULL)
	{
		Coll_tmp = Partie.Collision_head;
		return COLL_END;
	}

	static float elemPosY = 0;
	elemPosY = elem->PosY;

	//On avance dans la liste jusqu'à trouver un élément dont la différence des PosY est inférieure à MaximumRayonCollision
	while (Coll_tmp->next != NULL && elemPosY - Coll_tmp->elem->PosY > MaximumRayonCollision)
	{
		Coll_tmp = Coll_tmp->next;
	}

	//On vérifie que l'on ne dépasse plus MaximumRayonCollision, sinon le parcours est terminé
	if (Coll_tmp->next != NULL && Coll_tmp->elem->PosY - elemPosY > MaximumRayonCollision)
	{
		Coll_tmp = Partie.Collision_head;
		return COLL_END;
	}

	if (Coll_tmp->elem == elem)
	{
		Coll_last = Coll_tmp;
		Coll_tmp = Coll_tmp->next;
		return COLL_OK;
	}

	bool ResultColl = false;

	if (elem->ModeCollision == MODE_COLLISION_CERCLE && Coll_tmp->elem->ModeCollision == MODE_COLLISION_CERCLE)
		ResultColl = Collision_cercle_cercle(elem->PosX, elemPosY, elem->RayonCollision,
											 Coll_tmp->elem->PosX, Coll_tmp->elem->PosY, Coll_tmp->elem->RayonCollision);
	if (elem->ModeCollision == MODE_COLLISION_CERCLE && Coll_tmp->elem->ModeCollision == MODE_COLLISION_RECT)
		ResultColl = Collision_cercle_rectangle(elem->PosX, elemPosY, elem->RayonCollision,
												Coll_tmp->elem->PosX, Coll_tmp->elem->PosY, Coll_tmp->elem->RayX, Coll_tmp->elem->RayY);
	if (elem->ModeCollision == MODE_COLLISION_RECT && Coll_tmp->elem->ModeCollision == MODE_COLLISION_CERCLE)
		ResultColl = Collision_cercle_rectangle(Coll_tmp->elem->PosX, Coll_tmp->elem->PosY, Coll_tmp->elem->RayonCollision,
												elem->PosX, elemPosY, elem->RayX, elem->RayY);
	if (elem->ModeCollision == MODE_COLLISION_RECT && Coll_tmp->elem->ModeCollision == MODE_COLLISION_RECT)
		ResultColl = Collision_rectangle_rectangle(elem->PosX, elem->PosY, elem->RayX, elem->RayY,
												   Coll_tmp->elem->PosX, Coll_tmp->elem->PosY, Coll_tmp->elem->RayX, Coll_tmp->elem->RayY);

	if (ResultColl)
	{
		//On a une collision entre les deux rayons primaires des éléments

		int Coll = Coll_tmp->elem->Collision(elem, COLL_PRIM);
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
		Coll_last = Coll_tmp;
		Coll_tmp = Coll_tmp->next;
		return Retour;
	}

	//La suite teste les collisions en interaction et en vision : cela ne concerne pas les tmp paysages
	//Attention : seuls les paysages en RECT_COL sont bloqués ici ; un moyen pour les bloquer tous ?
	if (elem->Get_RayonInteraction() == 0 || elem->ModeCollision == MODE_COLLISION_RECT)
	{
		Coll_last = Coll_tmp;
		Coll_tmp = Coll_tmp->next;
		return COLL_OK;
	}

	//Pas de collision primaire ; on teste une collision en interaction
	ResultColl = Collision_cercle_cercle(elem->PosX, elemPosY, elem->Get_RayonInteraction(),
										 Coll_tmp->elem->PosX, Coll_tmp->elem->PosY, Coll_tmp->elem->RayonCollision);
	if (ResultColl)
	{
		if (TestAngle(elem->PosX, elemPosY, elem->Get_Dir(), Coll_tmp->elem->PosX, Coll_tmp->elem->PosY, elem->Get_NombreDir()))
			Retour = COLL_INTER;
		else Retour = COLL_INTER_ARR;
	}

	//Dans le cas de diplomatie identique, on s'arrête là
	if (elem->Diplomatie == Coll_tmp->elem->Diplomatie)
	{
		Coll_last = Coll_tmp;
		Coll_tmp = Coll_tmp->next;
		return Retour;
	}

	//Dans le cas de la diplomatie adverse, on transforme l'interaction en attaque
	if (Coll_tmp->elem->Diplomatie != DIPLOM_NEUTRE && Retour == COLL_INTER) Retour = COLL_ATT;
	if (Coll_tmp->elem->Diplomatie != DIPLOM_NEUTRE && Retour == COLL_INTER_ARR) Retour = COLL_ATT_ARR;

	//Pas de collision primaire, pas de collision en interaction ; on teste une collision en vision
	if (Retour != COLL_INTER && Retour != COLL_INTER_ARR && Retour != COLL_ATT && Retour != COLL_ATT_ARR &&
		elem->Get_ChampVision() != 0)
	{ /** DONC, POUR LE MOMENT, Retour == COLL_OK **/
		ResultColl = Collision_cercle_cercle(elem->PosX, elemPosY, elem->Get_ChampVision(),
											 Coll_tmp->elem->PosX, Coll_tmp->elem->PosY, Coll_tmp->elem->RayonCollision);
		if (ResultColl) Retour = COLL_VIS;
	}

	Coll_last = Coll_tmp;
	Coll_tmp = Coll_tmp->next;

	return Retour;
}
