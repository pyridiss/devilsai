
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

#include "tools/signals.h"
#include "tools/math.h"
#include "tools/timeManager.h"

#include "devilsai-resources/Carte.h"
#include "ElementsCarte.h"

#include "gamedata.h"

int Joueur::Gestion()
{
		// 1. Définition des variables

	bool MouvementAutorise = true;

		// 2. Vérifie que le personnage est toujours en vie !

	if (currentHealthStatus(Life) <= 0)
	{
        Set_Activite(behavior(Behaviors::Dying));
        if ((int)_animationFrame == 8) tools::signals::addSignal("player-dead");
	}

    Gestion_Recuperation();
	Gestion_Statistiques();

		// 3. Gestion du temps : Mouvement ou pas

    _timer = tools::timeManager::I(1/20.0);

    string skillToUse = behavior(Behaviors::Random);
    if (MouvementAutorise && !Set_Activite(behavior(Behaviors::Random)))
    {
        _hunting = false;
        _automove = false;
    }

    if (_hunting)
    {
        if (_hunted != nullptr && _hunted->currentHealthStatus(Life) > 0)
        {
            tools::math::Shape& s = skill(_skillForHunted)->interactionField;
            tools::math::Shape& s2 = (s.profile != tools::math::Shape::None) ? s : interactionField;

            if (tools::math::intersection(s2, _hunted->size))
            {
                _hunting = false;
                _automove = false;
                Set_Activite(_skillForHunted);
            }
            else
            {
                automove(_hunted->position());
            }
        }
        else _hunting = false;
    }

    if (_automove)
    {
        _fakeIndividual.move(_automoveEndpoint.x - _fakeIndividual.position().x, _automoveEndpoint.y - _fakeIndividual.position().y);
        skillToUse = behavior(Behaviors::Hunting);

        //Check if we are close enough to the endPoint
        if (tools::math::Vector2d::distance(_automoveEndpoint, position()) < 1.2 * skill(skillToUse)->step)
        {
            _automove = false;
            Set_Activite(behavior(Behaviors::Random));
        }
    }

    bool findNewPosition = false;
    if (skill(skillToUse)->step > 0 && MouvementAutorise) findNewPosition = true;

    seenItems = gamedata::currentWorld()->findAllCollidingItems(this, viewField, false);

    tools::math::Vector2d oldPosition = position();

    if (findNewPosition)
    {
        Set_Activite(skillToUse);
        if (_hunting)
            MouvementChasse(_hunted, 16, false);
        else
            MouvementChasse(&_fakeIndividual, 16, false);

        bool collision = false;
        int Iteration2 = 0;
        while (Iteration2 < 5)
        {
            collision = false;
            Element_Carte* other;
            for (auto& i : seenItems)
            {
                if (tools::math::intersection(size, i.first->size))
                {
                    int c = i.first->Collision(this, false);
                    if (c == COLL_PRIM || c == COLL_PRIM_MVT)
                    {
                        collision = true;
                        other = i.first;
                        break;
                    }
                }
            }
            if (collision == false) break;

            double angleWithOther = tools::math::angle(other->position().x - position().x, other->position().y - position().y);
            move(-2 * cos(angleWithOther), -2 * sin(angleWithOther));
            ++Iteration2;
        }

        if (collision)
        {
            move(oldPosition.x - position().x, oldPosition.y - position().y);
            Set_Activite(behavior(Behaviors::Blocked));
        }
    }

    _targetedItem = nullptr;
    double angleDifference = 100;
    for (auto& i : seenItems)
    {
        if (tools::math::intersection(size, i.first->size))
        {
            i.first->Collision(this, true);
        }
        if (tools::math::intersection(_currentSkill->interactionField, i.first->size))
        {
            if (abs(tools::math::angle(i.first->position().x - position().x, i.first->position().y - position().y) - angle) < angleDifference)
            {
                angleDifference = abs(tools::math::angle(i.first->position().x - position().x, i.first->position().y - position().y) - angle);
                _targetedItem = i.first;
            }
        }
    }

    //The individual selected by the used is more important than the one found by the collision manager
    if (selectedIndividual != nullptr && selectedIndividual != this)
        _targetedItem = selectedIndividual;

	if (MouvementAutorise)
	{
        nextAnimationFrame();

        interactionField.updateDirection(angle);
        _currentSkill->interactionField.updateDirection(angle);
	}

	return 0;
}
