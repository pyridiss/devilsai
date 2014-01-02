
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
#include "ElementsCarte.h"


/** FONCTIONS DE LA CLASSE Element_Carte **/

Element_Carte::Element_Carte()
{
	next = NULL;
}

Element_Carte::~Element_Carte()
{
	if (next != NULL) delete next;
	next = NULL;
}

short Element_Carte::Get_Controle()
{
	return Controle;
}

void Element_Carte::Lag_Pos(float x, float y)
{
	PosX += x;
	PosY += y;
}

void Element_Carte::Set_Controle(short nv)
{
	Controle = nv;
}

int Element_Carte::Gestion()
{
	if (Get_Controle() == HUMAIN) return ETAT_CONTINUER;

	if (Abs(Partie.PosCarteX - PosX) >= Options.ScreenW + 200)
	{
		Set_Controle(ARRET);
		return ETAT_DESACTIVE;
	}
	else
	{
		if (Abs(Partie.PosCarteY - PosY) >= Options.ScreenH + 400)
		{
			Set_Controle(ARRET);
			return ETAT_DESACTIVE;
		}
		else
		{
			if (Abs(Partie.PosCarteX - PosX) >= Options.ScreenW + 100)
			{
				Set_Controle(AI_IMG_HORSCHAMP);
			}
			else
			{
				if (Abs(Partie.PosCarteY - PosY) >= Options.ScreenH + 200)
					Set_Controle(AI_IMG_HORSCHAMP);
				else Set_Controle(AI);
			}
		}
	}
	return ETAT_CONTINUER;
}


/** FONCTIONS DE LA CLASSE Element_Mouvant **/

short Element_Mouvant::Get_Act()
{
	return Act;
}

short Element_Mouvant::Get_Dir()
{
	return Dir;
}

short Element_Mouvant::Get_Num()
{
	return Num;
}

bool Element_Mouvant::Set_Activite(int nv)
{
	if (Act == -1)
	{
		Act = nv;
		Num = 0;
	}

	if (Act == MORT) return false;

	if (Get_Activite(nv) == NULL) return false;

	if (Get_Activite(Act)->priority > Get_Activite(nv)->priority && (!ActEffectue || Num != 0)) return false;

	//La direction en cours est-elle compatible avec la nouvelle activité ?
	if (Get_Activite(nv)->Num_Max[Dir] == -1)
	{
		Dir = NORD;
		while (Get_Activite(nv)->Num_Max[Dir] == -1)
		{
			++Dir;
			if (Dir == 8) return false; //Maximum atteint
		}
	}

	if (Act == nv) return true;

	Act = nv;

	if (Act == MORT) RayonCollision = 0;

	if (Get_Activite(nv)->priority > 0) IncrementNum(true);
	return true;
}

void Element_Mouvant::Set_Dir(int nv)
{
	Dir = nv;
}

int Element_Mouvant::Collision(Individu *elem, int TypeCollision)
{
	if (TypeCollision == COLL_VIS)
	{
		if (elem->Diplomatie != DIPLOM_NEUTRE && elem->Diplomatie != Diplomatie)
		{
			if (elem->Comportement <= COMPORTEMENT_CHASSE) return COMPORTEMENT_CHASSE;
		}
		return COMPORTEMENT_ALEATOIRE;
	}
	return COLL_PRIM_MVT;
}

void Element_Mouvant::IncrementNum(bool RaZ)
{
	if (Act == MORT && Num == Get_Activite(Act)->Num_Max[Dir]-1) return;

	if (RaZ)
	{
		Num = 0;
		ActEffectue = false;
	}
	else
	{
		++Num;
		if (Num == Get_Activite(Act)->Num_Max[Dir]) Num = 0;
		ActEffectue = true;
	}
}

int Element_Mouvant::Gestion()
{
	if (Temps < (1/(float)Get_Vitesse(Get_Act())))
	{
		Temps += I(1/60.);
		return ETAT_NORMAL;
	}
	else Temps = I(1/60.);

	return ETAT_CONTINUER;
}

int Element_Mouvant::Get_Vitesse(short act)
{
	return Get_Activite(act)->speed;
}