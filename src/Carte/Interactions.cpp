
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

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../Jeu/Jeu.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "../Bibliotheque/Templates.h"

#include "gamedata.h"

void Combat(Individu *Attaquant, Individu *Blesse, lua_State *L)
{
	/*
		0. Echec ou succès ? (Agilite + Vitalite)
		1. Dégats de l'attaquant (Puissance ou Force + Vitalite + Intelligence)
		2. Parade ? (Esquive + Vitalite + Charisme attaquant)
		3. Résistance (Toutes les résistances + Vitalite)
	*/

	if (Attaquant == NULL || Blesse == NULL)
	{
		Erreur("Combat() :", "appelée avec l'un des deux individus NULL");
		return;
	}

	// On empêche (pour le moment) d'attaquer les alliés
	if (Attaquant->Diplomatie == Blesse->Diplomatie) return;

	float Att_Agilite = (float)(Attaquant->get("Agilite"));
	float Att_Intelli = (float)(Attaquant->get("Intelligence"));
	float Att_Puissance = (float)(Attaquant->get("Puissance"));
	if (Att_Agilite == 0) Att_Agilite = 1;
	if (Att_Intelli == 0) Att_Intelli = 1;
	if (Att_Puissance == 0) Att_Puissance = 1;

	float TauxReussite = (1 + (Att_Agilite - (float)Blesse->get("Agilite"))/Att_Agilite) * 50;
	if (TauxReussite > 95) TauxReussite = 95;
	if (TauxReussite < 5) TauxReussite = 5;

	bool Succes = (rand()%100 < TauxReussite) ? true : false;

	if (Succes)
	{
		float Degats = Attaquant->get("Force");
		if (L != NULL) Degats += getDoubleFromLUA(L, "getDegats") - getDoubleFromLUA(L, "getAmplitude") + rand()%(2*getIntFromLUA(L, "getAmplitude"));

		float TauxEsquive = (1 + ((float)Blesse->get("Esquive") - Att_Agilite)/Att_Agilite)*50;
		bool Esquive = (rand()%100 < TauxEsquive) ? true : false;

		if (Esquive)
		{
			Degats = 0;
			if (Blesse->Get_Controle() == HUMAIN)
			{
				gamedata::player()->CoupEsquive(Attaquant);
				Ajouter_LignePerso(getTranslatedMessage(_ESQUIVE), Color(128, 255, 128, 255));
			}
		}
		else
		{
			float TauxCritique = (1 + (Att_Intelli - (float)Blesse->get("Charisme"))/Att_Intelli)*10;
			bool Critique = (rand()%100 < TauxCritique) ? true : false;
			if (Critique)
			{
				Degats *= 1.5;
				Attaquant->Lag_Energie(-10);
				Blesse->Lag_Recuperation(-30);
				if (Attaquant->Id == gamedata::player()->Id)
				{
					gamedata::player()->CoupCritique(Blesse);
					Ajouter_LignePerso(getTranslatedMessage(_CRITIQUE), Color(240, 40, 40, 255));
				}
				if (Blesse->Id == gamedata::player()->Id)
				{
					gamedata::player()->BlessureGrave(Blesse);
				}
			}

			Degats *= (1 + (Att_Puissance - (float)Blesse->get("Constitution"))/Att_Puissance)/2;
			if (Degats < 5) Degats = 5;
		}

		Blesse->Lag_Vitalite(-Degats);
		Blesse->Lag_Recuperation(-3-Degats/20);

		if (Attaquant->Get_Controle() == HUMAIN)
		{
			gamedata::player()->GainExperience(Blesse, Degats);

			String32 Infliges = getFormatedTranslatedMessage(_DEGATS_INFLIGES, (unsigned)Degats);
			Ajouter_LignePerso(Infliges, Color(0, 0, 0, 255));
		}
		if (Blesse->Id == gamedata::player()->Id)
		{
			if (gamedata::player()->Get_Act() == BLESSE) gamedata::player()->BlessuresMultiples(Attaquant);
			String32 Recus = getFormatedTranslatedMessage(_DEGATS_RECUS, (unsigned)Degats);
			Ajouter_LignePerso(Recus, Color(0, 0, 0, 255));
		}

		if (Degats) Blesse->Set_Activite(BLESSE);
	}
	else if (Attaquant->Id == gamedata::player()->Id) Ajouter_LignePerso(getTranslatedMessage(_ECHEC), Color(200, 10, 20, 255));
}
