
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

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "../Carte/Carte.h"
#include "ElementsCarte.h"

#include "gamedata.h"

/** FONCTIONS DE GESTION DE LA CLASSE Individu **/

int Individu::Gestion()
{
	//Test de proximité au joueur
	int Retour = Element_Carte::Gestion();
	if (Retour != ETAT_CONTINUER) return Retour;

	// 0. Vérifie que l'individu n'est pas mort...

    if (_currentSkill->Id == behavior(Behaviors::Dying) && Get_Num() == _currentSkill->numberOfImages-1)
	{
        createCorpse();
        return ETAT_MORT;
	}
    if (_currentSkill->Id != behavior(Behaviors::Dying) && (currentHealthStatus(Life) == 0 || lifetime == 0))
	{
        if (!Set_Activite(behavior(Behaviors::Dying))) return ETAT_MORT;
		else return ETAT_NORMAL;
	}

	Gestion_Recuperation();

	//Gestion du temps :
	Retour = Individu::GestionElementMouvant();
	if (Retour != ETAT_CONTINUER) return Retour;

    if (_currentSkill->Id == behavior(Behaviors::Dying))
	{
        Set_Activite(behavior(Behaviors::Dying)); //Permet de gérer les situations spéciales (dépose des inventaires, etc…)
		IncrementNum();
		return ETAT_NORMAL;
	}

	if (NouveauComportement != -1) Comportement = NouveauComportement;

	int Iteration = 0;
    Element_Carte* tmp = NULL;
    string attackToUse;

    bool findNewPosition = false;

    Element_Carte *Elem = nullptr;

    if (ElementVision != -1)
        Elem = gamedata::findElement(ElementVision);

    if (Elem == nullptr)
	{
		ElementVision = -1;
        Comportement = Behaviors::Random;
	}

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
                MouvementChasse(Elem);
                break;
            case Behaviors::Attacking:
                angle = tools::math::angle(Elem->position().x - position().x, Elem->position().y - position().y);
                break;
            case Behaviors::REGEN :
                MouvementChasse(Elem);
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

    for (auto& i : seenItems)
    {
        if (tools::math::intersection(size, i.first->size))
        {
            i.first->Collision(this, true);
        }
        switch (i.second)
        {
            case COLL_PRIM_MVT:
                if (i.first->Diplomatie != 0 && i.first->Diplomatie != Diplomatie)
                {
                    bool canUseAttack = false;
                    for (auto& attack : attacks())
                    {
                        if (skill(attack) == nullptr)
                        {
                            tools::debug::error("This skill does not exists : " + Type + ":" + attack, "items", __FILENAME__, __LINE__);
                            continue;
                        }

                        skill(attack)->interactionField.setOrigin(&position());
                        skill(attack)->interactionField.updateDirection(angle);

                        if (tools::math::intersection(skill(attack)->interactionField, i.first->size))
                        {
                            canUseAttack = true;
                            if (Behaviors::Attacking > NouveauComportement)
                            {
                                NouveauComportement = Behaviors::Attacking;
                                tmp = i.first;
                                attackToUse = attack;
                            }
                        }
                    }

                    if (!canUseAttack)
                    {
                        if (Behaviors::Hunting > NouveauComportement && !angleFixed())
                        {
                            NouveauComportement = Behaviors::Hunting;
                            tmp = i.first;
                        }
                        else if (NouveauComportement == Behaviors::Hunting)
                        {
                            if (tools::math::Vector2d::distance(position(), i.first->position()) < tools::math::Vector2d::distance(position(), tmp->position()))
                                tmp = i.first;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }

    if (tmp != NULL)
    {
        ElementVision = tmp->Id;
        Elem = tmp;
    }

    if (NouveauComportement == Behaviors::Attacking)
        Set_Activite(attackToUse);

	IncrementNum();

	if (Get_Num() == 0 && NouveauComportement == Behaviors::Attacking)
	{
		Individu *ennemi = dynamic_cast<Individu*>(Elem);

		//On vérifie que l'ennemi est toujours à portée, avant de lancer le combat :
		if (ennemi != NULL)
		{
            if (tools::math::intersection(_currentSkill->interactionField, ennemi->size))
				Combat(this, ennemi);
            NouveauComportement = Behaviors::Hunting;
		}
        else NouveauComportement = Behaviors::Random;
	}
    if (Get_Num() == 0)
    {
        _currentSkill->atEnd(this);
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

    move(cos(angle)*_currentSkill->step, sin(angle)*_currentSkill->step);
}

bool Individu::MouvementChasse(Element_Carte *elem)
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
            findPath(elem->position());
            validityOfPath.circle(elem->position(), 20);
        }
        else
            validityOfPath.circle(elem->position(), 5);
    }

    if (pathToTarget.profile != tools::math::Shape::Profiles::None && pathToTarget.points.size() >= 2)
    {
        tools::math::Vector2d step = pathToTarget.points[pathToTarget.points.size() - 2];
        angle = tools::math::angle(step.x - position().x, step.y - position().y);
        move(cos(angle)*_currentSkill->step, sin(angle)*_currentSkill->step);
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
