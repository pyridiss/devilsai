
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
#include "tools/timeManager.h"
#include "tools/math.h"

#include "devilsai-resources/Carte.h"
#include "ElementsCarte.h"

#include "gamedata.h"

/** FONCTIONS DE GESTION DE LA CLASSE Individu **/

int Individu::Gestion()
{
    if (Element_Carte::Gestion() == ETAT_MORT)
        return ETAT_MORT;

    if (_currentSkill.none()) return ETAT_NORMAL;

	// 0. Vérifie que l'individu n'est pas mort...

    if (_currentSkill == skill(behavior(Behaviors::Dying)) && _animationFrame >= _currentSkill->numberOfImages-1)
	{
        createCorpse();
        return ETAT_MORT;
	}
    if (_currentSkill != skill(behavior(Behaviors::Dying)) && (currentHealthStatus(Life) == 0 || lifetime == 0))
	{
        if (!Set_Activite(behavior(Behaviors::Dying))) return ETAT_MORT;
		else return ETAT_NORMAL;
	}

	Gestion_Recuperation();

	//Gestion du temps :
    double speed = currentHealthStatus(_currentSkill->speedAttribute);
    _timer += tools::timeManager::I(1/20.0);
    if (_timer * speed < ((_currentSkill->step > 0) ? _currentSkill->step : 10))
        return ETAT_NORMAL;

    if (_currentSkill == skill(behavior(Behaviors::Dying)))
	{
        Set_Activite(behavior(Behaviors::Dying)); //Permet de gérer les situations spéciales (dépose des inventaires, etc…)
        nextAnimationFrame();
		return ETAT_NORMAL;
	}

	if (NouveauComportement != -1) Comportement = NouveauComportement;

	int Iteration = 0;
    string attackToUse;

    bool findNewPosition = false;



    if (_targetedItem == nullptr)
        Comportement = Behaviors::Random;

    Set_Activite(behavior(Comportement));

    if (_currentSkill->step > 0) findNewPosition = true;

    seenItems = gamedata::currentWorld()->findAllCollidingItems(this, viewField, false);

    tools::math::Vector2d oldPosition = position();

    while (findNewPosition)
    {
        switch (Comportement)
        {
            case Behaviors::Random :
                MouvementAleatoire(Iteration);
                break;
            case Behaviors::Hunting :
                MouvementChasse(_targetedItem, 8, true);
                break;
            case Behaviors::Attacking:
                angle = tools::math::angle(_targetedItem->position().x - position().x, _targetedItem->position().y - position().y);
                break;
            case Behaviors::LookingForLife :
                MouvementChasse(_targetedItem, 8, true);
                break;
            default:
                break;
        }

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
            //Le mouvement précédent a entraîné une collision primaire : retour en arrière.
            move(oldPosition.x - position().x, oldPosition.y - position().y);

            //If cannot reach the hunted item, it is better to stop than to stomp.
            if (Comportement == Behaviors::Hunting)
                Iteration = 4;

            if (Iteration == 4) //Aucun mouvement valable n'a été trouvé
            {
                findNewPosition = false;
                Set_Activite(behavior(Behaviors::Blocked));
            }
        }
        else
            findNewPosition = false;

        ++Iteration;
    }

    NouveauComportement = Behaviors::Random;
    _targetedItem = nullptr;

    for (auto& i : seenItems)
    {
        if (tools::math::intersection(size, i.first->size))
        {
            i.first->Collision(this, true);
        }
        switch (i.second)
        {
            case COLL_PRIM_MVT:
                if (i.first->Diplomatie != DIPLOM_NEUTRE && i.first->Diplomatie != DIPLOM_BREAKABLE && i.first->Diplomatie != Diplomatie)
                {
                    bool canUseAttack = false;
                    for (auto& attack : attacks())
                    {
                        if (skill(attack).none())
                        {
                            tools::debug::error("This skill does not exists : " + Type + ":" + attack, "items", __FILENAME__, __LINE__);
                            continue;
                        }
                        if (skill(attack).unavailability() > 0)
                            continue;

                        skill(attack)->interactionField.setOrigin(&position());
                        skill(attack)->interactionField.updateDirection(angle);

                        if (tools::math::intersection(skill(attack)->interactionField, i.first->size))
                        {
                            canUseAttack = true;
                            if (Behaviors::Attacking > NouveauComportement)
                            {
                                NouveauComportement = Behaviors::Attacking;
                                _targetedItem = i.first;
                                attackToUse = attack;
                            }
                        }
                    }

                    if (!canUseAttack)
                    {
                        if (Behaviors::Hunting > NouveauComportement && !angleFixed())
                        {
                            NouveauComportement = Behaviors::Hunting;
                            _targetedItem = i.first;
                        }
                        else if (NouveauComportement == Behaviors::Hunting)
                        {
                            if (tools::math::Vector2d::distance(position(), i.first->position()) < tools::math::Vector2d::distance(position(), _targetedItem->position()))
                                _targetedItem = i.first;
                        }
                    }
                }
                break;
            case COLL_LIFE:
                if (currentHealthStatus(Life) < 150 || currentHealthStatus(Healing) < -15)
                {
                    if (Behaviors::LookingForLife > NouveauComportement)
                    {
                        NouveauComportement = Behaviors::LookingForLife;
                        _targetedItem = i.first;
                    }
                }
            default:
                break;
        }
    }


    if (NouveauComportement == Behaviors::Attacking)
        Set_Activite(attackToUse);

    nextAnimationFrame();

    if (ActEffectue && NouveauComportement == Behaviors::Attacking)
	{
		Individu *ennemi = dynamic_cast<Individu*>(_targetedItem);

		//On vérifie que l'ennemi est toujours à portée, avant de lancer le combat :
		if (ennemi != NULL)
		{
            if (tools::math::intersection(_currentSkill->interactionField, ennemi->size))
                fight(ennemi);
            NouveauComportement = Behaviors::Hunting;
		}
        else NouveauComportement = Behaviors::Random;
	}

	return ETAT_NORMAL;
}

void Individu::MouvementAleatoire(bool newDirection)
{
    if (!angleFixed())
    {
        if (newDirection)
            angle = tools::math::randomNumber(0.0, 2.0 * M_PI);
        angle += tools::math::randomNumber_BinomialLaw(-10.0, 10.0);
    }

    while (angle < 0) angle += 2.0 * M_PI;
    while (angle > 2.0 * M_PI) angle -= 2.0 * M_PI;

    double distanceTraveled = currentHealthStatus(_currentSkill->speedAttribute) * _timer;
    move(cos(angle) * distanceTraveled, sin(angle) * distanceTraveled);
}

bool Individu::MouvementChasse(Element_Carte *elem, int nodesNumber, bool reduceCollisionWithIndividuals)
{
    if (!tools::math::intersection(elem->size, validityOfPath))
    {
        pathToTarget.line(elem->position(), position(), size.radius1);
        pathToTarget.setOrigin(tools::math::absoluteOrigin());
        bool obstacle = false;
        for (auto& i : seenItems)
        {
            if (i.first == elem) continue;
            if (tools::math::intersection(pathToTarget, i.first->size))
            {
                obstacle = true;
                break;
            }
        }

        if (obstacle)
        {
            findPath(elem->position(), nodesNumber, reduceCollisionWithIndividuals);
            validityOfPath.circle(elem->position(), 20);
        }
        else
            validityOfPath.circle(elem->position(), 5);
    }

    if (pathToTarget.profile != tools::math::Shape::Profiles::None && pathToTarget.points.size() >= 2)
    {
        tools::math::Vector2d step = pathToTarget.points[pathToTarget.points.size() - 2];
        angle = tools::math::angle(step.x - position().x, step.y - position().y);
        double distanceTraveled = currentHealthStatus(_currentSkill->speedAttribute) * _timer;
        move(cos(angle) * distanceTraveled, sin(angle) * distanceTraveled);
        if (tools::math::Vector2d::distanceSquare(step, position()) < 100)
            pathToTarget.points.pop_back();
    }
    else
    {
        Set_Activite(behavior(Behaviors::Blocked));
		return false;
	}

	return true;
}
