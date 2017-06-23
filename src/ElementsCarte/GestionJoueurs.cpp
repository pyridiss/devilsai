
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

#include <lua5.2/lua.hpp>

#include "tools/signals.h"
#include "tools/math.h"

#include "../Bibliotheque/Constantes.h"
#include "../Carte/Carte.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

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
	static short Competence = COMPETENCE_AUCUNE;
	static short ChoixCompetence = COMPETENCE_AUCUNE;

		// 2. Vérifie que le personnage est toujours en vie !

	if (get("Vitalite") <= 0)
	{
		Set_Activite(MORT);
		for (int a = 0 ; a < 4 ; ++a)
		{
			Appui[a] = false; Old[a] = false;
		}
        if (Get_Num() == 8) tools::signals::addSignal("player-dead");
	}

	Gestion_Equipement();
	Gestion_Statistiques();

		// 3. Gestion du temps : Mouvement ou pas

	if (Element_Mouvant::Gestion() != ETAT_CONTINUER)
		MouvementAutorise = false;

		// 4. Gestion des événements

	if (get("Vitalite") > 0 && Jeu.App.hasFocus())
	{
        Appui[HAUT]   = Keyboard::isKeyPressed(Keyboard::Up);
        Appui[BAS]    = Keyboard::isKeyPressed(Keyboard::Down);
        Appui[DROITE] = Keyboard::isKeyPressed(Keyboard::Right);
        Appui[GAUCHE] = Keyboard::isKeyPressed(Keyboard::Left);

				//Activité (pas de touche) ou Compétence :
        ChoixCompetence = COMPETENCE_AUCUNE;
        if (Keyboard::isKeyPressed(Keyboard::LControl)) ChoixCompetence = COMPETENCE_CTRL;
        if (Keyboard::isKeyPressed(Keyboard::LShift))   ChoixCompetence = COMPETENCE_SHIFT;
        if (Keyboard::isKeyPressed(Keyboard::Tab))      ChoixCompetence = COMPETENCE_TAB;
        if (Keyboard::isKeyPressed(Keyboard::Space))    ChoixCompetence = COMPETENCE_SPACE;
	}

	//Désactivation de (presque) tout si on est en mode cinématiques
	if (Partie.ModeCinematiques)
	{
		Appui[HAUT] = false; Appui[BAS] = false; Appui[DROITE] = false; Appui[GAUCHE] = false;
		ChoixCompetence = COMPETENCE_AUCUNE;
	}

	//Enchaîne les activités sans attendre que la précédente soit terminée et détermine la compétence à activer
	for(int a = 0 ; a < 4 ; ++a)
	{
		if (Appui[a] != Old[a]) Temps = 1;
		Old[a] = Appui[a];
	}
    if (ChoixCompetence != COMPETENCE_AUCUNE && skillLinks[ChoixCompetence] != nullptr && Competence != ChoixCompetence)
	{
		if (getBoolFromLUA(skillLinks[ChoixCompetence], "testEnoughEnergy") == true)
		{
			Competence = ChoixCompetence;
			Temps = 1;
		}
		else Disp_Information(_ENERGIE_INSUFFISANTE, true);
	}

		// 5. Modification de la position

	float modif_maj = 1;

	int TabAppui = 10000*(Competence+1) + 1000*Appui[NORD] + 100*Appui[SUD] + 10*Appui[EST] + Appui[OUEST];

    //Be careful: the Y axis is inversed
	switch (TabAppui%10000)
	{
		case 1		: angle = M_PI; break;
		case 10		: angle = 0; break;
		case 100	: angle = M_PI / 2; break;
		case 101	: angle = 3 * M_PI / 4; break;
		case 110	: angle = M_PI / 4; break;
		case 1000	: angle = 3 * M_PI / 2; break;
		case 1001	: angle = 5 * M_PI / 4; break;
		case 1010	: angle = 7 * M_PI / 4; break;
		default :	break; //Aucune touche, Competence seule, ou touches superflues (prend l'ancienne config)
	}
	if (Get_Act() == MORT) angle = 3 * M_PI / 2;

    if (TabAppui%10000 == 0 && automove)
    {
        angle = tools::math::angle(automoveEndpoint.x - position().x, automoveEndpoint.y - position().y);
        TabAppui += 1; //TabToAct() will think we are holding an arrow key.

        //Check if we are close enough to the endPoint
        if (abs(automoveEndpoint.x - position().x) + abs(automoveEndpoint.y - position().y) < 15)
            modif_maj = 0.25; //Last step will be divided by four
        if (abs(automoveEndpoint.x - position().x) + abs(automoveEndpoint.y - position().y) < 5)
            automove = false;
    }
    else automove = false;


	int Resultat = COLL_OK;
	int EnAttente = 0;
	int Iteration = 0;
	float maj = 0;
	Element_Carte* tmp1 = NULL;

	while(Resultat != COLL_END && MouvementAutorise)
	{
		//GESTION DES MOUVEMENTS
		if (Resultat == COLL_PRIM)
		{
			//Le mouvement précédent a entraîné une collision primaire : retour en arrière.
			Lag_PosFondCartes(position().x - Partie.PosCarteX, position().y - Partie.PosCarteY);

            move(Partie.PosCarteX - position().x, Partie.PosCarteY - position().y);

			switch (Iteration)
			{
				case 1 :	modif_maj = 0.5; break;
				case 2 :	modif_maj = 0.25; break;
				case 3 :	modif_maj = 1;
							move(sin(angle) * 7, 0); Lag_PosFondCartes(-sin(angle) * 7, 0); break;
				case 4 :	move(-sin(angle) * 14, 0); Lag_PosFondCartes(sin(angle) * 14, 0); break;
				case 5 :	move(sin(angle) * 7, -cos(angle) * 7); Lag_PosFondCartes(-sin(angle) * 7, cos(angle) * 7); break;
				case 6 :	move(0, cos(angle) * 14); Lag_PosFondCartes(0, -cos(angle) * 14); break;
				case 7 :	move(0, -cos(angle) * 7); Lag_PosFondCartes(0, cos(angle) * 7); break;
			}
			if (Iteration >= 8) //Aucun mouvement valable n'a été trouvé
			{
				Resultat = COLL_END;
				Set_Activite(PAUSE);
				modif_maj = 0;
                automove = false;
			}
		}

		Set_Activite(PAUSE);

		if (Iteration > 8) break;

		if (Iteration < 8)
		{
			Set_Activite(TabToAct(TabAppui));
			for (int c = 0 ; c < NOMBRE_COMPETENCES ; ++c)
			{
                if (skillLinks[c] != nullptr)
				{
					if (Competence == c)
						setBoolToLUA(skillLinks[c], "setActivated", true);
					else
						setBoolToLUA(skillLinks[c], "setActivated", false);
				}
			}
		}

		//Nouveau mouvement
		maj = Get_Activite(Get_Act())->step * modif_maj;

		Lag_PosFondCartes(-cos(angle)*maj, -sin(angle)*maj);

		move(cos(angle)*maj, sin(angle)*maj);

		//TESTS DE COLLISIONS
		Resultat = COLL_OK;
		Partie.CarteCourante->resetCollisionManager();
		ElementInteraction = -1;

		while(Resultat != COLL_END && Resultat != COLL_PRIM)
		{
			Resultat = Partie.CarteCourante->browseCollisionList(this);

			if (Resultat == COLL_PRIM_MVT)
			{
				Resultat = COLL_PRIM;
			}

			if (Resultat == COLL_INTER)
			{
				if (EnAttente != COLL_ATT)
				{
					tmp1 = Partie.CarteCourante->getCurrentCollider();
					EnAttente = COLL_INTER;
					if (tmp1 != NULL) ElementInteraction = tmp1->Id;
				}
				Resultat = COLL_OK;
			}

			if (Resultat == COLL_ATT)
			{
				tmp1 = Partie.CarteCourante->getCurrentCollider();
				EnAttente = COLL_ATT;
				if (tmp1 != NULL) ElementInteraction = tmp1->Id;
			}
		} //Fin du while de parcours de la liste de collisions

		++Iteration;

	} //Fin du while d'AI

	if (MouvementAutorise)
	{
		Set_PosCarte(position().x, position().y, false);

		IncrementNum();

		if (Get_Num() == 0)
		{
			for (int c = 0 ; c < NOMBRE_COMPETENCES ; ++c)
                if (skillLinks[c] != nullptr && Get_Act() == getStringFromLUA(skillLinks[c], "getActivite"))
					setBoolToLUA(skillLinks[c], "setActiviteFinished", true);
		}

		if (EnAttente == COLL_INTER)
		{
			if (tmp1 != NULL && (tmp1->Type == "TYPE_COFFRE" || tmp1->Type == "TYPE_CADAVRE"))
			{
				if (Partie.CoffreOuvert == NULL) Partie.CoffreOuvert = dynamic_cast<Coffre*>(tmp1);
				else if (Partie.CoffreOuvert->Id != tmp1->Id) Partie.CoffreOuvert = dynamic_cast<Coffre*>(tmp1);
				if (Partie.CoffreOuvert != NULL) Partie.currentUserScreen = &(Partie.screenEquipment);
			}
		}
		else
		{
			if (Partie.CoffreOuvert != NULL) Partie.currentUserScreen = nullptr;
			Partie.CoffreOuvert = NULL;
		}

		for (int c = 0 ; c < NOMBRE_COMPETENCES ; ++c)
            if (skillLinks[c] != nullptr)
			{
				lua_getglobal(skillLinks[c], "use");
				lua_call(skillLinks[c], 0, 0);
			}

		//Stop using skills if the current one is finished
		if (Competence != COMPETENCE_AUCUNE && getBoolFromLUA(skillLinks[Competence], "getActivated") == false)
			Competence = COMPETENCE_AUCUNE;
	}

	return 0;
}
