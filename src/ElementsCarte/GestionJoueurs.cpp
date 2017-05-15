
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

#include "../Bibliotheque/Constantes.h"
#include "../Carte/Carte.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"


int Joueur::Gestion()
{
		// 1. Définition des variables
	int Retour = ACTION_JEU;

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
		if (Get_Num() == 8) Retour = ACTION_MORT;
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
		if (Retour != ACTION_QUITTER) Retour = ACTION_JEU;
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

	switch (TabAppui%10000)
	{
		case 1		: Dir = OUEST; break;
		case 10		: Dir = EST; break;
		case 100	: Dir = SUD; break;
		case 101	: Dir = S_O; break;
		case 110	: Dir = S_E; break;
		case 1000	: Dir = NORD; break;
		case 1001	: Dir = N_O; break;
		case 1010	: Dir = N_E; break;
		default :	break; //Aucune touche, Competence seule, ou touches superflues (prend l'ancienne config)
	}
	if (Get_Act() == MORT) Dir = SUD;

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
			Lag_PosFondCartes(PosX - Partie.PosCarteX, PosY - Partie.PosCarteY);

			PosX = Partie.PosCarteX;
			PosY = Partie.PosCarteY;

			switch (Iteration)
			{
				case 1 :	modif_maj = 0.5; break;
				case 2 :	modif_maj = 0.25; break;
				case 3 :	modif_maj = 1;
							PosX += DirToCoeff_Y(Dir) * 7; Lag_PosFondCartes(-DirToCoeff_Y(Dir) * 7, 0); break;
				case 4 :	PosX -= DirToCoeff_Y(Dir) * 14; Lag_PosFondCartes(DirToCoeff_Y(Dir) * 14, 0); break;
				case 5 :	PosX += DirToCoeff_Y(Dir) * 7; PosY -= DirToCoeff_X(Dir) * 7; Lag_PosFondCartes(-DirToCoeff_Y(Dir) * 7, DirToCoeff_X(Dir) * 7); break;
				case 6 :	PosY += DirToCoeff_X(Dir) * 14; Lag_PosFondCartes(0, -DirToCoeff_X(Dir) * 14); break;
				case 7 :	PosY -= DirToCoeff_X(Dir) * 7; Lag_PosFondCartes(0, DirToCoeff_X(Dir) * 7); break;
			}
			if (Iteration >= 8) //Aucun mouvement valable n'a été trouvé
			{
				Resultat = COLL_END;
				Set_Activite(PAUSE);
				modif_maj = 0;
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

		Lag_PosFondCartes(-DirToCoeff_X(Dir)*maj, -DirToCoeff_Y(Dir)*maj);

		PosX += DirToCoeff_X(Dir)*maj;
		PosY += DirToCoeff_Y(Dir)*maj;

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
			if (Resultat == COLL_PRIM && maj == 0)
			{
				//On tente de détecter le cas où deux individus seraient bloqués l'un sur l'autre
				//La solution n'est pas optimale, mais on s'en contente pour le moment…
				Element_Carte* tmp2 = Partie.CarteCourante->getCurrentCollider();

				while ((abs(PosX - tmp2->PosX) < tmp2->RayonCollision && abs(PosY - tmp2->PosY) < tmp2->RayonCollision) ||
						(abs(PosX - tmp2->PosX) < tmp2->RayX && abs(PosY - tmp2->PosY) < tmp2->RayY))
				{
					PosX += 2*DirToCoeff_X(Dir)*RayonCollision;
					PosY += 2*DirToCoeff_Y(Dir)*RayonCollision;
					Set_PosCarte(PosX, PosY, false);
					Set_PosCarte(PosX, PosY, true);
					Partie.CarteCourante->resetCollisionManager();
				}
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

			if (Resultat == COLL_INTER_ARR) Resultat = COLL_OK;

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
		Set_PosCarte(PosX, PosY, false);

		IncrementNum();

		if (Get_Num() == 0)
		{
			for (int c = 0 ; c < NOMBRE_COMPETENCES ; ++c)
                if (skillLinks[c] != nullptr && Get_Act() == getIntFromLUA(skillLinks[c], "getActivite"))
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

	Disp(PosX, PosY);

	//Désactive tous les déplacements et toutes les compétences avant la mise en pause ou la mort
	if (Retour == ACTION_PAUSE || Retour == ACTION_MORT)
	{
		for (int a = 0 ; a < 4 ; ++a)
		{
			Appui[a] = false; Old[a] = false;
		}
		Competence = COMPETENCE_AUCUNE;
		ChoixCompetence = COMPETENCE_AUCUNE;
	}

	return Retour;
}
