
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

    if (Get_Act() == behavior(Behaviors::Dying) && Get_Num() == Get_Activite(Get_Act())->numberOfImages-1)
	{
			return ETAT_MORT;
	}
    if (Get_Act() != behavior(Behaviors::Dying) && (currentHealthStatus(Statistiques::Life) == 0 || lifetime == 0))
	{
        if (!Set_Activite(behavior(Behaviors::Dying))) return ETAT_MORT;
		else return ETAT_NORMAL;
	}

	Gestion_Recuperation();

	//Gestion du temps :
	Retour = Individu::GestionElementMouvant();
	if (Retour != ETAT_CONTINUER) return Retour;

    if (Get_Act() == behavior(Behaviors::Dying))
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

    if (Get_Activite(Act)->step > 0) findNewPosition = true;

    seenItems = gamedata::currentWorld()->findAllCollidingItems(this, viewField, false);

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
            case Behaviors::REGEN :
                MouvementChasse(Elem);
                break;
            default:
                break;
        }

        bool collision = false;
        for (auto& i : seenItems)
        {
            if (tools::math::intersection(size, i.first->size))
            {
                int c = i.first->Collision(this, false);
                if (c == COLL_PRIM || c == COLL_PRIM_MVT)
                {
                    collision = true;
                    break;
                }
            }
        }

        if (collision)
        {
            //Le mouvement précédent a entraîné une collision primaire : retour en arrière.
            move(-cos(angle)*Get_Activite(Act)->step, -sin(angle)*Get_Activite(Act)->step);

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
                        if (Get_Activite(attack) == nullptr)
                        {
                            tools::debug::error("This skill does not exists : " + Type + ":" + attack, "items", __FILENAME__, __LINE__);
                            continue;
                        }

                        Get_Activite(attack)->interactionField.setOrigin(&position());
                        Get_Activite(attack)->interactionField.updateDirection(angle);

                        if (tools::math::intersection(Get_Activite(attack)->interactionField, i.first->size))
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
                        if (Behaviors::Hunting > NouveauComportement)
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

    if (tmp != NULL) ElementVision = tmp->Id;

    if (NouveauComportement == Behaviors::Attacking)
        Set_Activite(attackToUse);

	IncrementNum();

	if (Get_Num() == 0 && NouveauComportement == Behaviors::Attacking)
	{
		Individu *ennemi = dynamic_cast<Individu*>(Elem);

		//On vérifie que l'ennemi est toujours à portée, avant de lancer le combat :
		if (ennemi != NULL)
		{
            if (tools::math::intersection(Get_Activite(Act)->interactionField, ennemi->size))
				Combat(this, ennemi);
            NouveauComportement = Behaviors::Hunting;
		}
        else NouveauComportement = Behaviors::Random;
	}
    if (Get_Num() == 0)
    {
        Get_Activite(Act)->atEnd(this);
    }

	return ETAT_NORMAL;
}

void Individu::MouvementAleatoire(int Iteration)
{
    if (!angleFixed())
    {
        angle += M_PI / 2.0 * Iteration;
        angle += tools::math::randomNumber_BinomialLaw(-10.0, 10.0);
    }

    while (angle < 0) angle += 2.0 * M_PI;
    while (angle > 2.0 * M_PI) angle -= 2.0 * M_PI;

    move(cos(angle)*Get_Activite(Act)->step, sin(angle)*Get_Activite(Act)->step);
}

bool Individu::MouvementChasse(Element_Carte *elem)
{
	int Iteration = 0;

	if (elem == NULL)
	{
		Erreur("Individu::MouvementChasse() a été appelée avec un élément NULL", "");
		return true;
	}

    if (!angleFixed())
        angle = tools::math::angle(elem->position().x - position().x, elem->position().y - position().y);

    while(Iteration < 10)
    {
        angle += (double)Iteration * M_PI / 4.0;

        while (angle < 0) angle += 2.0 * M_PI;
        while (angle > 2.0 * M_PI) angle -= 2.0 * M_PI;

        move(cos(angle)*Get_Activite(Act)->step, sin(angle)*Get_Activite(Act)->step);

        //Tests de collision :
        int Resultat = COLL_OK;
        gamedata::currentWorld()->resetCollisionManager();
        while(Resultat != COLL_END && Resultat != COLL_PRIM)
        {
            Resultat = gamedata::currentWorld()->browseCollisionList(this);

            //Annihile COLL_PRIM_MVT si c'est l'élément chassé qui est détecté
            if (Resultat == COLL_PRIM_MVT && gamedata::currentWorld()->getCurrentCollider() == elem) Resultat = COLL_OK;

            //Les Collisions INTER sont pour le moment inutiles ici
            if (Resultat == COLL_INTER) Resultat = COLL_OK;

            if (Resultat == COLL_PRIM_MVT) Resultat = COLL_PRIM;
            if (Resultat == COLL_ATT) Resultat = COLL_OK;
        }
        if (Resultat == COLL_PRIM)
        {
            move(-cos(angle)*Get_Activite(Act)->step, -sin(angle)*Get_Activite(Act)->step);
            ++Iteration;
        }
        else
        {
            break;
        }
    }

	//Pas de mouvement trouvé : PAUSE.
    if (Iteration == 10)
	{
        Set_Activite(behavior(Behaviors::Blocked));
		return false;
	}

	return true;
}
