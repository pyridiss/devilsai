
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

	if (Get_Act() == MORT && Get_Num() == Get_Activite(Get_Act())->numberOfImages-1)
	{
			return ETAT_MORT;
	}
	if (Get_Act() != MORT && (currentHealthStatus(Statistiques::Life) == 0 || lifetime == 0))
	{
		if (!Set_Activite(MORT)) return ETAT_MORT;
		else return ETAT_NORMAL;
	}

	Gestion_Recuperation();

	//Gestion du temps :
	Retour = Individu::GestionElementMouvant();
	if (Retour != ETAT_CONTINUER) return Retour;

	if (Get_Act() == MORT)
	{
		Set_Activite(MORT); //Permet de gérer les situations spéciales (dépose des inventaires, etc…)
		IncrementNum();
		return ETAT_NORMAL;
	}

	if (NouveauComportement != -1) Comportement = NouveauComportement;

	int Resultat = COLL_OK;
	int EnAttente = 0;
	int Iteration = 0;
	Element_Carte* tmp1 = NULL, *tmp2 = NULL;

    Element_Carte *Elem = nullptr;

    if (ElementVision != -1)
        Elem = gamedata::findElement(ElementVision);

    if (Elem == nullptr)
	{
		ElementVision = -1;
		Comportement = COMPORTEMENT_ALEATOIRE;
	}

	while(Resultat != COLL_END)
	{
		Resultat = COLL_OK;

		if (!Set_Activite(ActDefaut) && Get_Num() != Get_Activite(Act)->numberOfImages)
		{
			Resultat = COLL_END;
			EnAttente = -1;
			//En cas d'attaque, on cherche quand même la meilleure direction
			if (Act == ATTAQUE && Elem != NULL)
			{
                if (!angleFixed())
                    angle = tools::math::angle(Elem->position().x - position().x, Elem->position().y - position().y);
			}
			break;
		}

		//Déplacement selon le Comportement de l'individu
		switch (Comportement)
		{
			case COMPORTEMENT_ALEATOIRE :	MouvementAleatoire(Iteration); break;
            case COMPORTEMENT_CHASSE :		if (tools::math::intersection(interactionField, Elem->size))
											{
												Resultat = COLL_END; EnAttente = COLL_ATT;
												if (Elem->Diplomatie != DIPLOM_NEUTRE && Elem->Diplomatie != Diplomatie)
												{
													NouveauComportement = COMPORTEMENT_ATTAQUE;
												}
											}
											else
												MouvementChasse(Elem);
											break;
			case COMPORTEMENT_ATTAQUE :		break;
			case COMPORTEMENT_REGEN :		MouvementChasse(Elem); break;
		}

		//Test de collision dans la nouvelle position de l'individu
		NouveauComportement = -1;
		gamedata::currentWorld()->resetCollisionManager();
		while(Resultat != COLL_END && Resultat != COLL_PRIM)
		{
			Resultat = gamedata::currentWorld()->browseCollisionList(this);

			if (Resultat == COLL_PRIM_MVT) Resultat = COLL_PRIM;

			//Les Collisions INTER sont pour le moment inutiles ici
			if (Resultat == COLL_INTER) Resultat = COLL_OK;

			if (Resultat == COLL_ATT)
			{
				tmp2 = gamedata::currentWorld()->getCurrentCollider();
				if (tmp1 == NULL || EnAttente == COLL_VIS) tmp1 = tmp2;
				EnAttente = COLL_ATT;
				NouveauComportement = COMPORTEMENT_ATTAQUE;
			}
			if (Resultat == COLL_VIS)
			{
				tmp2 = gamedata::currentWorld()->getCurrentCollider();
				if (tmp2 == NULL) break;

				Resultat = tmp2->Collision(this, COLL_VIS); //Permet de définir, si nécessaire, le nouveau comportement

				if (Resultat > NouveauComportement)
				{
					NouveauComportement = Resultat;
					tmp1 = tmp2;
					EnAttente = COLL_VIS;
				}
				if (Resultat == NouveauComportement)
				{
					if (tmp1 == NULL) tmp1 = tmp2;

					if ((position().x-tmp2->position().x)*(position().x-tmp2->position().x)+(position().y-tmp2->position().y)*(position().y-tmp2->position().y) < (position().x-tmp1->position().x)*(position().x-tmp1->position().x)+(position().y-tmp1->position().y)*(position().y-tmp1->position().y))
						tmp1 = tmp2;

					EnAttente = COLL_VIS;
				}
			}
		} //Fin du while de parcours de la liste de collisions

		if (Resultat == COLL_PRIM)
		{
			//Le mouvement précédent a entraîné une collision primaire : retour en arrière.
			move(-cos(angle)*Get_Activite(Act)->step, -sin(angle)*Get_Activite(Act)->step);

			if (Iteration == 4) //Aucun mouvement valable n'a été trouvé
			{
				Resultat = COLL_END;
				Set_Activite(PAUSE);
				break;
			}
		}

		++Iteration;
	}	//Fin du while d'AI

	if (EnAttente == COLL_VIS || EnAttente == COLL_ATT)
	{
		if (tmp1 != NULL) ElementVision = tmp1->Id;
		else if (tmp2 != NULL) ElementVision = tmp2->Id;
	}
	if (EnAttente == COLL_ATT && ElementVision != -1) 
	{
		Set_Activite(ATTAQUE);
	}
	if (EnAttente == 0) NouveauComportement = COMPORTEMENT_ALEATOIRE;

	IncrementNum();

	if (Get_Num() == 0 && Get_Act() == ATTAQUE)
	{
		Individu *ennemi = dynamic_cast<Individu*>(Elem);

		//On vérifie que l'ennemi est toujours à portée, avant de lancer le combat :
		if (ennemi != NULL)
		{
			if (tools::math::intersection(interactionField, ennemi->size))
				Combat(this, ennemi);
			NouveauComportement = COMPORTEMENT_CHASSE;
		}
		else NouveauComportement = COMPORTEMENT_ALEATOIRE;
	}
    if (Get_Num() == 0)
    {
        Get_Activite(Act)->atEnd(this);
    }

	return ETAT_NORMAL;
}

void Individu::MouvementAleatoire(int Iteration)
{
    Set_Activite(ActDefaut);

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
	int buf = 0;
	int DirActuelle = 0;
	int Iteration = 0;
	int coeff = 1;

	if (elem == NULL)
	{
		Erreur("Individu::MouvementChasse() a été appelée avec un élément NULL", "");
		return true;
	}

    if (!angleFixed())
        angle = tools::math::angle(elem->position().x - position().x, elem->position().y - position().y);

    Set_Activite(COURSE);

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
		Set_Activite(PAUSE);
		return false;
	}

	return true;
}
