
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

#include <lua.hpp>

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "tools/math.h"
#include "tools/aStar.h"
#include "imageManager/imageManager.h"

#include "Jeu/options.h"


Individu::Individu()
    : Element_Carte(),
    _species(nullptr),
    _displayedName(nullptr)
{
	TypeClassement = CLASSEMENT_NORMAL;
    angle = tools::math::randomNumber(0.0, 2 * M_PI);
    inert = false;
    interactionField.setOrigin(&position());
    viewField.setOrigin(&position());
    pathToTarget.profile = tools::math::Shape::Profiles::None;
    validityOfPath.circle(tools::math::Vector2d(0, 0), 20);
    validityOfPath.setOrigin(tools::math::absoluteOrigin());
}

Individu::~Individu()
{
    if (_displayedName != nullptr)
        delete _displayedName;
}

const String32& Individu::displayedName()
{
    if (_displayedName != nullptr)
        return *_displayedName;

    if (_species != nullptr)
        return _species->_displayedName;

    //In case there is no custom name nor species...
    setCustomDisplayedName(String32());
    return *_displayedName;
}

unsigned int Individu::experience()
{
    if (_species != nullptr)
        return _experience + _species->_experience;

    return _experience;
}

bool Individu::angleFixed()
{
    if (_species != nullptr)
        return _species->angleFixed;

    return false;
}

void Individu::Gestion_Recuperation()
{
    modifyHealthStatus(Life, currentHealthStatus(Healing)/1000.0 * tools::timeManager::I(1.0));
    modifyHealthStatus(Energy, currentHealthStatus(Healing)/1000.0 * tools::timeManager::I(1.0));

    if (currentHealthStatus(Healing) > 95)
    {
        modifyHealthStatus(Life, tools::timeManager::I(1.0));
        modifyHealthStatus(Energy, tools::timeManager::I(1.0));
    }

    double diff = currentHealthStatus(HealingPower) + (currentHealthStatus(Life)-800.0)/100.0 - currentHealthStatus(Healing);
    modifyHealthStatus(Healing, diff / 1000.0 * tools::timeManager::I(1.0));

    //Test de récupération forcée (potion, …)
    if (RecuperationFixe || abs(currentHealthStatus(HealingPower)) >= 95)
        setHealthStatus(Healing, currentHealthStatus(HealingPower));

    if (EnergieMax) setHealthStatus(Energy, 1000);

    //Diminue la durée de vie des objets utilisés
    for (mapObjects::iterator i = inventory.objects.begin() ; i != inventory.objects.end() ; ++i)
    {
        if (getStringFromLUA(i->second, "getIdEmplacement") == i->first && getDoubleFromLUA(i->second, "getDuree") > 0)
        {
            setDoubleToLUA(i->second, "setDuree", getDoubleFromLUA(i->second, "getDuree") - tools::timeManager::I(1));

            if (getDoubleFromLUA(i->second, "getDuree") <= 0)
            {
                lua_State *j = i->second;
                i = inventory.objects.erase(i);
                inventory.deleteObject(j);
                continue;
            }
        }
    }
}

int Individu::currentHealthStatus(Attribute a, bool forceUpdate)
{
    //Update values
    if (forceUpdate || _clock.getElapsedTime().asSeconds() > 1)
    {
        _currentHealthStatus = _attributes;

        for (int i = Strength ; i != numberOfAttributes ; ++i)
        {
            Attribute att = static_cast<Attribute>(i);
            AttributeAmplifier attAmplifier = static_cast<AttributeAmplifier>(i);

            if (att != HealingPower)
                _currentHealthStatus.set(att, _currentHealthStatus[att] / 2.0 * (1 + 1.2*_currentHealthStatus[Life]/1000.0));

            for (auto& o : inventory.objects)
            {
                lua_getglobal(o.second, "getCurrentObjectEffect");
                lua_pushstring(o.second, attributeToString(att));
                lua_call(o.second, 1, 1);
                _currentHealthStatus.add(att, lua_tonumber(o.second, -1));
                lua_pop(o.second, 1);
                lua_getglobal(o.second, "getCurrentObjectEffect");
                lua_pushstring(o.second, attributeAmplifierToString(attAmplifier));
                lua_call(o.second, 1, 1);
                _currentHealthStatus.add(att, _attributes[att] * lua_tonumber(o.second, -1) / 100.0);
                lua_pop(o.second, 1);
            }
        }
        _clock.restart();
    }

    //Return asked value
    return floor(_currentHealthStatus[a]);
}

void Individu::setHealthStatus(Attribute a, double value)
{
    if (a == Life || a == Energy)
    {
        value = min(max(0.0, value), 1000.0);
        _currentHealthStatus.set(a, value);
        _attributes.set(a, value);
    }
    if (a == Healing)
    {
        value = min(max(-100.0, value), 100.0);
        _currentHealthStatus.set(a, value);
        _attributes.set(a, value);
    }
}

void Individu::modifyHealthStatus(Attribute a, double value)
{
    if (a == Life || a == Energy)
    {
        value = min(max(-_currentHealthStatus[a], value), 1000.0 - _currentHealthStatus[a]);
        _currentHealthStatus.add(a, value);
        _attributes.add(a, value);
    }
    if (a == Healing)
    {
        value = min(max(-100.0 - _currentHealthStatus[a], value), 100.0 - _currentHealthStatus[a]);
        _currentHealthStatus.add(a, value);
        _attributes.add(a, value);
    }
}

void Individu::setSpecies(Classe_Commune* s)
{
    _species = s;
}

Classe_Commune* Individu::species()
{
    return _species;
}

void Individu::setCustomDisplayedName(const String32& newName)
{
    if (_displayedName == nullptr)
        _displayedName = new String32;

    *_displayedName = newName;
}

void Individu::updateAngle(const tools::math::Vector2d& p)
{
    angle = tools::math::angle(p.x - position().x, p.y - position().y);
}

void Individu::findPath(const tools::math::Vector2d& destination)
{
    tools::aStar::clear();

    tools::aStar::setPoints(position(), destination);
    tools::aStar::setField(viewField);

    tools::math::Shape node;
    node.circle(tools::math::Vector2d(0, 0), size.radius1);
    tools::aStar::setNodesProperties(node, 8, 30, 150);

    vector<pair<tools::math::Shape*, int>> obstacles;
    for (auto& i : seenItems)
    {
        switch (i.second)
        {
            case COLL_PRIM :
                obstacles.emplace_back(&i.first->size, 100000);
                break;
            case COLL_PRIM_MVT :
                obstacles.emplace_back(&i.first->size, 3000);
                break;
            default:
                break;
        }
    }

    tools::aStar::setObstacles(obstacles);

    pathToTarget = tools::aStar::aStar();
    pathToTarget.setOrigin(tools::math::absoluteOrigin());
    pathToTarget.radius1 = size.radius1;
}

void Individu::Disp(RenderTarget& target)
{
    if (options::option<bool>(tools::math::sdbm_hash("displayShapes")))
    {
        viewField.display(target, Color(255, 255, 0, 20));
        interactionField.display(target, Color(255, 0, 0, 30));
        size.display(target, Color(255, 255, 255, 50));
    }

	Activite* act = Get_Activite(Act);
    imageManager::display(target, "individuals", act->getImageKey(angle, Num), position().x, position().y, true);
}

void Individu::displayLifeGauge(RenderTarget& target)
{
    if (Diplomatie == DIPLOM_NEUTRE) return;

    //x and y are used to convert position to integers; otherwise, the text may be blurred
    int x = position().x;
    int y = position().y;

    Color color;
    if (Diplomatie == DIPLOM_ALLIE) color = Color(128, 255, 128, 255);
    if (Diplomatie == DIPLOM_ENNEMI) color = Color(255, 255, 255, 255);
    Disp_TexteCentre(displayedName(), x, y + 25, color, 10.f);

    RectangleShape background(Vector2f(50, 4));
    background.setPosition(x - 25, y + 35);
    background.setFillColor(Color(0, 0, 0, 175));
    target.draw(background);

    RectangleShape foreground(Vector2f(currentHealthStatus(Life)/20, 4));
    foreground.setPosition(x - 25, y + 35);
    foreground.setFillColor(Color(228, 0, 0, 255));
    target.draw(foreground);
}
