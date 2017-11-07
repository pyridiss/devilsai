
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

#include "tools/signals.h"
#include "tools/math.h"
#include "tools/textManager.h"

#include "../Bibliotheque/Constantes.h"
#include "../Carte/Carte.h"
#include "../Jeu/Jeu.h"
#include "Jeu/options.h"
#include "ElementsCarte.h"

#include "gamedata.h"

#define NORD 0
#define SUD 1
#define EST 2
#define OUEST 3
#define N_E 4
#define S_O 5
#define N_O 6
#define S_E 7

int Joueur::Gestion()
{
		// 1. Définition des variables

	bool MouvementAutorise = true;
	static bool Appui[4] = {false};
	static bool Old[4] = {false};

		// 2. Vérifie que le personnage est toujours en vie !

	if (currentHealthStatus(Statistiques::Life) <= 0)
	{
        Set_Activite(behavior(Behaviors::Dying));
		for (int a = 0 ; a < 4 ; ++a)
		{
			Appui[a] = false; Old[a] = false;
		}
        if (Get_Num() == 8) tools::signals::addSignal("player-dead");
	}

    Gestion_Recuperation();
	Gestion_Statistiques();

		// 3. Gestion du temps : Mouvement ou pas

	if (Individu::GestionElementMouvant() != ETAT_CONTINUER)
		MouvementAutorise = false;

		// 4. Gestion des événements

	if (currentHealthStatus(Statistiques::Life) > 0 && Jeu.App.hasFocus())
	{
        Appui[HAUT]   = Keyboard::isKeyPressed(Keyboard::Up);
        Appui[BAS]    = Keyboard::isKeyPressed(Keyboard::Down);
        Appui[DROITE] = Keyboard::isKeyPressed(Keyboard::Right);
        Appui[GAUCHE] = Keyboard::isKeyPressed(Keyboard::Left);
	}

	//Désactivation de (presque) tout si on est en mode cinématiques
	if (options::option<bool>("cinematic-mode"))
	{
		Appui[HAUT] = false; Appui[BAS] = false; Appui[DROITE] = false; Appui[GAUCHE] = false;
	}

	//Enchaîne les activités sans attendre que la précédente soit terminée et détermine la compétence à activer
	for(int a = 0 ; a < 4 ; ++a)
	{
		if (Appui[a] != Old[a]) Temps = 1;
		Old[a] = Appui[a];
	}

    if (Act != "3") _automove = false;

    if (_hunting)
    {
        Individu* ind = dynamic_cast<Individu*>(gamedata::findElement(_hunted));
        if (ind != nullptr && ind->Act != behavior(Behaviors::Dying))
        {
            tools::math::Shape& s = Get_Activite(_skillForHunted)->interactionField;
            tools::math::Shape& s2 = (s.profile != tools::math::Shape::None) ? s : interactionField;

            if (tools::math::intersection(s2, ind->size))
            {
                _hunting = false;
                Set_Activite(_skillForHunted);
            }
            else
            {
                automove(ind->position());
            }
        }
        else _hunting = false;
    }

		// 5. Modification de la position

	float modif_maj = 1;

	int TabAppui = 1000*Appui[NORD] + 100*Appui[SUD] + 10*Appui[EST] + Appui[OUEST];

    //Be careful: the Y axis is inversed
	switch (TabAppui)
	{
		case 1		: angle = M_PI; break;
		case 10		: angle = 0; break;
		case 100	: angle = M_PI / 2; break;
		case 101	: angle = 3 * M_PI / 4; break;
		case 110	: angle = M_PI / 4; break;
		case 1000	: angle = 3 * M_PI / 2; break;
		case 1001	: angle = 5 * M_PI / 4; break;
		case 1010	: angle = 7 * M_PI / 4; break;
		default :	break; //Aucune touche ou touches superflues (prend l'ancienne config)
	}
	if (Get_Act() == MORT) angle = 3 * M_PI / 2;

    if (TabAppui == 0 && _automove)
    {
        angle = tools::math::angle(_automoveEndpoint.x - position().x, _automoveEndpoint.y - position().y);
        TabAppui += 1; //TabToAct() will think we are holding an arrow key.

        //Check if we are close enough to the endPoint
        if (tools::math::Vector2d::distance(_automoveEndpoint, position()) < 1.2 * Get_Activite(Act)->step)
            _automove = false;
    }
    else _automove = false;


	int Resultat = COLL_OK;
	int EnAttente = 0;
	int Iteration = 0;
	float maj = 0;
    double oldPositionX = position().x;
    double oldPositionY = position().y;
	Element_Carte* tmp1 = NULL;

	while(Resultat != COLL_END && MouvementAutorise)
	{
		//GESTION DES MOUVEMENTS
		if (Resultat == COLL_PRIM)
		{
			//Le mouvement précédent a entraîné une collision primaire : retour en arrière.
// 			Lag_PosFondCartes(position().x - Partie.PosCarteX, position().y - Partie.PosCarteY);

            move(oldPositionX - position().x, oldPositionY - position().y);

			switch (Iteration)
			{
				case 1 :	modif_maj = 0.5; break;
				case 2 :	modif_maj = 0.25; break;
				case 3 :	modif_maj = 1;
							move(sin(angle) * 7, 0); break;
				case 4 :	move(-sin(angle) * 14, 0); break;
				case 5 :	move(sin(angle) * 7, -cos(angle) * 7); break;
				case 6 :	move(0, cos(angle) * 14); break;
				case 7 :	move(0, -cos(angle) * 7); break;
			}
			if (Iteration >= 8) //Aucun mouvement valable n'a été trouvé
			{
				Resultat = COLL_END;
				Set_Activite(PAUSE);
				modif_maj = 0;
                _automove = false;
			}
		}

		Set_Activite(PAUSE);

		if (Iteration > 8) break;

		if (Iteration < 8)
		{
			Set_Activite(TabToAct(TabAppui));
		}

		//Nouveau mouvement
		maj = Get_Activite(Get_Act())->step * modif_maj;

		move(cos(angle)*maj, sin(angle)*maj);

		//TESTS DE COLLISIONS
		Resultat = COLL_OK;
		gamedata::currentWorld()->resetCollisionManager();
		ElementInteraction = -1;

		while(Resultat != COLL_END && Resultat != COLL_PRIM)
		{
			Resultat = gamedata::currentWorld()->browseCollisionList(this);

			if (Resultat == COLL_PRIM_MVT)
			{
				Resultat = COLL_PRIM;
			}

			if (Resultat == COLL_INTER)
			{
				if (EnAttente != COLL_ATT)
				{
					tmp1 = gamedata::currentWorld()->getCurrentCollider();
					EnAttente = COLL_INTER;
					if (tmp1 != NULL) ElementInteraction = tmp1->Id;
				}
				Resultat = COLL_OK;
			}

			if (Resultat == COLL_ATT)
			{
				tmp1 = gamedata::currentWorld()->getCurrentCollider();
				EnAttente = COLL_ATT;
				if (tmp1 != NULL) ElementInteraction = tmp1->Id;
			}
		} //Fin du while de parcours de la liste de collisions

		++Iteration;

	} //Fin du while d'AI

    //The individual selected by the used is more important than the one found by the collision manager
    if (selectedIndividual != -1 && selectedIndividual != Id)
        ElementInteraction = selectedIndividual;

	if (MouvementAutorise)
	{
		IncrementNum();

        interactionField.updateDirection(angle);

		if (Get_Num() == 0)
		{
            Get_Activite(Act)->atEnd(this);
		}
	}

	return 0;
}
