
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

#include "tools/debug.h"
#include "tools/math.h"
#include "tools/timeManager.h"

#include "../Bibliotheque/Constantes.h"
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

int Element_Carte::Gestion()
{
    if (lifetime > 0)
    {
        lifetime -= tools::timeManager::I(1);
        if (lifetime < 0) lifetime = 0;
    }

	return ETAT_CONTINUER;
}

void Element_Carte::otherItemDeleted(Element_Carte* other)
{
}

void Element_Carte::move(double x, double y)
{
    _position.x += x;
    _position.y += y;
}

const tools::math::Vector2d& Element_Carte::position() const
{
    return _position;
}

/** FONCTIONS DE LA CLASSE Element_Mouvant **/

short Individu::Get_Num()
{
	return Num;
}

bool Individu::Set_Activite(string nv)
{
    if (skill(nv) == nullptr) return false;

	if (_currentSkill == nullptr)
	{
        _currentSkill = skill(nv);
        _currentSkill->atBegin(this);
		Num = 0;
	}

    if (_currentSkill->priority > skill(nv)->priority && (!ActEffectue || Num != 0)) return false;

    if (_currentSkill->Id == nv) return true;

    _currentSkill = skill(nv);
    _currentSkill->atBegin(this);

    if (_currentSkill->Id == behavior(Behaviors::Dying))
    {
        setHealthStatus(Life, 0);
        setHealthStatus(Energy, 0);
        setHealthStatus(Healing, 0);
        Diplomatie = DIPLOM_NEUTRE;
    }

    if (nv == behavior(Behaviors::Dying)) size.circle(tools::math::Vector2d(0, 0), 0);

    if (_currentSkill->priority > 0) IncrementNum(true);
	return true;
}

int Individu::Collision(Individu *elem, bool apply)
{
	return COLL_PRIM_MVT;
}

void Individu::IncrementNum(bool RaZ)
{
    if (_currentSkill->Id == behavior(Behaviors::Dying) && Num == _currentSkill->numberOfImages-1) return;

	if (RaZ)
	{
		Num = 0;
		ActEffectue = false;
	}
	else
	{
		++Num;
        if (Num == _currentSkill->numberOfImages) Num = 0;
		ActEffectue = true;
	}
}

int Individu::GestionElementMouvant()
{
    if (_currentSkill == nullptr) return 0;

    double speed = _currentSkill->speed + currentHealthStatus(_currentSkill->speedImprover);
    if (Temps < 1.0/speed)
	{
		Temps += tools::timeManager::I(1/60.);
		return ETAT_NORMAL;
	}
	else Temps = tools::timeManager::I(1/60.);

	return ETAT_CONTINUER;
}
