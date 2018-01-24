
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
#include "../ElementsCarte/ElementsCarte.h"

#include "tools/textManager.h"

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

    //Force the updating of attributes
    Attaquant->currentHealthStatus(Strength, true);
    Blesse->currentHealthStatus(Strength, true);

    double Att_Agilite = Attaquant->currentHealthStatus(Agility);
    double Att_Intelli = Attaquant->currentHealthStatus(Intellect);
    double Att_Puissance = Attaquant->currentHealthStatus(Power);
	if (Att_Agilite == 0) Att_Agilite = 1;
	if (Att_Intelli == 0) Att_Intelli = 1;
	if (Att_Puissance == 0) Att_Puissance = 1;

    double TauxReussite = (1.0 + (Att_Agilite - Blesse->currentHealthStatus(Agility))/Att_Agilite) * 50.0;
	if (TauxReussite > 95) TauxReussite = 95;
	if (TauxReussite < 5) TauxReussite = 5;

	bool Succes = (rand()%100 < TauxReussite) ? true : false;

	if (Succes)
	{
        double Degats = Attaquant->currentHealthStatus(Strength);
		if (L != NULL) Degats += getDoubleFromLUA(L, "getDegats") - getDoubleFromLUA(L, "getAmplitude") + rand()%(2*getIntFromLUA(L, "getAmplitude"));

        double TauxEsquive = (1.0 + (Blesse->currentHealthStatus(Dodge) - Att_Agilite)/Att_Agilite) * 50.0;
		bool Esquive = (rand()%100 < TauxEsquive) ? true : false;

		if (Esquive)
		{
			Degats = 0;
			if (Blesse == gamedata::player())
			{
				gamedata::player()->CoupEsquive(Attaquant);
				Ajouter_LignePerso(textManager::getText("devilsai", "ESQUIVE"), Color(128, 255, 128, 255));
			}
		}
		else
		{
            double TauxCritique = (1 + (Att_Intelli - Blesse->currentHealthStatus(Charisma))/Att_Intelli) * 10.0;
			bool Critique = (rand()%100 < TauxCritique) ? true : false;
			if (Critique)
			{
				Degats *= 1.5;
                Attaquant->modifyHealthStatus(Energy, -10);
                Blesse->modifyHealthStatus(Healing, -30);
				if (Attaquant->Id == gamedata::player()->Id)
				{
					gamedata::player()->CoupCritique(Blesse);
					Ajouter_LignePerso(textManager::getText("devilsai", "CRITIQUE"), Color(240, 40, 40, 255));
				}
				if (Blesse->Id == gamedata::player()->Id)
				{
					gamedata::player()->BlessureGrave(Blesse);
				}
			}

            Degats *= (1 + (Att_Puissance - Blesse->currentHealthStatus(Constitution))/Att_Puissance)/2.0;
			if (Degats < 5) Degats = 5;
		}

        Blesse->modifyHealthStatus(Life, -Degats);
        Blesse->modifyHealthStatus(Healing, -3-Degats/20);

        Attaquant->GainExperience(Blesse, Degats);

        textManager::PlainText resultText = textManager::getText("devilsai", "console-damageDone");

        textManager::PlainText a;
        if (Attaquant->Diplomatie == DIPLOM_ALLIE)
            a += "@c[128,255,128]";
        if (Attaquant->Diplomatie == DIPLOM_ENNEMI)
            a += "@c[255,128,128]";
        a += Attaquant->displayedName();

        textManager::PlainText b;
        if (Blesse->Diplomatie == DIPLOM_ALLIE)
            b += "@c[128,255,128]";
        if (Blesse->Diplomatie == DIPLOM_ENNEMI)
            b += "@c[255,128,128]";
        b += Blesse->displayedName();

        resultText.addParameter(a);
        resultText.addParameter((int)Degats);
        resultText.addParameter(b);

        Ajouter_LignePerso(resultText, Color(255, 255, 255));
		if (Attaquant == gamedata::player())
		{
            gamedata::player()->ApplicationAmeliorations();
		}
		if (Blesse == gamedata::player())
		{
            if (gamedata::player()->_currentSkill->Id == Blesse->behavior(Behaviors::Hurt)) gamedata::player()->BlessuresMultiples(Attaquant);
		}

        if (Degats) Blesse->Set_Activite(Blesse->behavior(Behaviors::Hurt));
	}
	else if (Attaquant->Id == gamedata::player()->Id) Ajouter_LignePerso(textManager::getText("devilsai", "ECHEC"), Color(200, 10, 20, 255));
}
