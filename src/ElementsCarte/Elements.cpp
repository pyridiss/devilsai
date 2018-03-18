
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
  : _position(),
    size(),
    inert(false)
{
    Id = tools::math::newUniqueIdentifier();
    size.setOrigin(&_position);
}

Element_Carte::Element_Carte(const Element_Carte& other)
  : Id(tools::math::newUniqueIdentifier()),
    Type(other.Type),
    Liste(other.Liste),
    Diplomatie(other.Diplomatie),
    lifetime(other.lifetime),
    _position(other._position),
    size(other.size),
    inert(other.inert),
    ignoreCollision(other.ignoreCollision),
    TypeClassement(other.TypeClassement)
{
    size.setOrigin(&_position);
}

Element_Carte::Element_Carte(Element_Carte&& other) noexcept
  : Id(other.Id),
    Type(std::move(other.Type)),
    Liste(std::move(other.Liste)),
    Diplomatie(other.Diplomatie),
    lifetime(other.lifetime),
    _position(std::move(other._position)),
    size(std::move(other.size)),
    inert(other.inert),
    ignoreCollision(other.ignoreCollision),
    TypeClassement(other.TypeClassement)
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

bool Individu::Set_Activite(string nv)
{
    if (skill(nv) == nullptr) return false;

	if (_currentSkill == nullptr)
	{
        _currentSkill = skill(nv);
        _currentSkill->atBegin(this);
        _animationFrame = 0;
	}

    if (_currentSkill->priority > skill(nv)->priority && (!ActEffectue || _animationFrame != 0)) return false;

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

    if (_currentSkill->priority > 0) nextAnimationFrame(true);
	return true;
}

bool Individu::Set_Activite(Behaviors b)
{
    return Set_Activite(behavior(b));
}

int Individu::Collision(Individu *elem, bool apply)
{
	return COLL_PRIM_MVT;
}

void Individu::nextAnimationFrame(bool RaZ)
{
    if (_currentSkill->Id == behavior(Behaviors::Dying) && _animationFrame == _currentSkill->numberOfImages-1) return;

	if (RaZ)
	{
        _animationFrame = 0;
		ActEffectue = false;
	}
	else
	{
        ++_animationFrame;
        if (_animationFrame == _currentSkill->numberOfImages) _animationFrame = 0;
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
