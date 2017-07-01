
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

#include "tools/math.h"
#include "tools/timeManager.h"

#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"


/** FONCTIONS DE LA CLASSE Element_Carte **/

Element_Carte::Element_Carte()
{
    Id = tools::math::newUniqueIdentifier();
    _position.x = 0;
    _position.y = 0;
    size.setOrigin(&_position);
}

Element_Carte::~Element_Carte()
{
}

short Element_Carte::Get_Controle()
{
	return Controle;
}

void Element_Carte::Set_Controle(short nv)
{
	Controle = nv;
}

int Element_Carte::Gestion()
{
	if (Get_Controle() == HUMAIN) return ETAT_CONTINUER;

    if (lifetime > 0)
    {
        lifetime -= tools::timeManager::I(1);
        if (lifetime < 0) lifetime = 0;
    }

	return ETAT_CONTINUER;
}

void Element_Carte::move(double x, double y)
{
    _position.x += x;
    _position.y += y;
}

const tools::math::Vector2d& Element_Carte::position()
{
    return _position;
}

/** FONCTIONS DE LA CLASSE Element_Mouvant **/

string Individu::Get_Act()
{
	return Act;
}

short Individu::Get_Num()
{
	return Num;
}

bool Individu::Set_Activite(string nv)
{
	if (Get_Activite(Act) == NULL)
	{
		Act = nv;
		Num = 0;
	}

	if (Act == MORT) return false;

	if (Get_Activite(nv) == NULL) return false;

	if (Get_Activite(Act)->priority > Get_Activite(nv)->priority && (!ActEffectue || Num != 0)) return false;

	if (Act == nv) return true;

	Act = nv;

	if (Act == MORT) size.circle(tools::math::Vector2d(0, 0), 0);

	if (Get_Activite(nv)->priority > 0) IncrementNum(true);
	return true;
}

int Individu::Collision(Individu *elem, int TypeCollision)
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

void Individu::IncrementNum(bool RaZ)
{
	if (Act == MORT && Num == Get_Activite(Act)->numberOfImages-1) return;

	if (RaZ)
	{
		Num = 0;
		ActEffectue = false;
	}
	else
	{
		++Num;
		if (Num == Get_Activite(Act)->numberOfImages) Num = 0;
		ActEffectue = true;
	}
}

int Individu::GestionElementMouvant()
{
	if (Temps < (1/(float)Get_Vitesse(Get_Act())))
	{
		Temps += tools::timeManager::I(1/60.);
		return ETAT_NORMAL;
	}
	else Temps = tools::timeManager::I(1/60.);

	return ETAT_CONTINUER;
}

int Individu::Get_Vitesse(string act)
{
	return Get_Activite(act)->speed;
}
