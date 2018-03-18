
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

#include "tools/debug.h"

#include "../ElementsCarte/ElementsCarte.h"

#include "textManager/textManager.h"

#include "devilsai-gui/console.h"

#include "gamedata.h"

void Individu::fight(Individu *enemy, string skillName)
{
	/*
		0. Echec ou succès ? (Agilite + Vitalite)
		1. Dégats de l'attaquant (Puissance ou Force + Vitalite + Intelligence)
		2. Parade ? (Esquive + Vitalite + Charisme attaquant)
		3. Résistance (Toutes les résistances + Vitalite)
	*/

    if (enemy == nullptr)
	{
        tools::debug::warning("The function fight() has been called with a nullptr", "", __FILENAME__, __LINE__);
		return;
	}

	// On empêche (pour le moment) d'attaquer les alliés
    if (Diplomatie == enemy->Diplomatie) return;

    //Force the updating of attributes
    currentHealthStatus(Strength, true);
    enemy->currentHealthStatus(Strength, true);

    double Att_Agilite = currentHealthStatus(Agility);
    double Att_Intelli = currentHealthStatus(Intellect);
    double Att_Puissance = currentHealthStatus(Power);
	if (Att_Agilite == 0) Att_Agilite = 1;
	if (Att_Intelli == 0) Att_Intelli = 1;
	if (Att_Puissance == 0) Att_Puissance = 1;

    double TauxReussite = (1.0 + (Att_Agilite - enemy->currentHealthStatus(Agility))/Att_Agilite) * 50.0;
	if (TauxReussite > 95) TauxReussite = 95;
	if (TauxReussite < 5) TauxReussite = 5;

	bool Succes = (rand()%100 < TauxReussite) ? true : false;

	if (Succes)
	{
        double Degats = currentHealthStatus(Strength);
        if (!skillName.empty())
        {
            double d = skill(skillName)->damage();
            double a = skill(skillName)->amplitude();
            Degats += d - a + rand()%(2*int(a));
        }

        double TauxEsquive = (1.0 + (enemy->currentHealthStatus(Dodge) - Att_Agilite)/Att_Agilite) * 50.0;
		bool Esquive = (rand()%100 < TauxEsquive) ? true : false;

		if (Esquive)
		{
			Degats = 0;
            if (enemy == gamedata::player())
			{
				gamedata::player()->CoupEsquive(this);
                addConsoleEntry(textManager::getText("devilsai", "ESQUIVE"));
			}
		}
		else
		{
            double TauxCritique = (1 + (Att_Intelli - enemy->currentHealthStatus(Charisma))/Att_Intelli) * 10.0;
			bool Critique = (rand()%100 < TauxCritique) ? true : false;
			if (Critique)
			{
				Degats *= 1.5;
                modifyHealthStatus(Energy, -10);
                enemy->modifyHealthStatus(Healing, -30);
                if (Id == gamedata::player()->Id)
				{
					gamedata::player()->CoupCritique(enemy);
                    addConsoleEntry(textManager::getText("devilsai", "CRITIQUE"));
				}
                if (enemy->Id == gamedata::player()->Id)
				{
					gamedata::player()->BlessureGrave(enemy);
				}
			}

            Degats *= (1 + (Att_Puissance - enemy->currentHealthStatus(Constitution))/Att_Puissance)/2.0;
			if (Degats < 5) Degats = 5;
		}

        enemy->modifyHealthStatus(Life, -Degats);
        enemy->modifyHealthStatus(Healing, -3-Degats/20);

        GainExperience(enemy, Degats);

        textManager::PlainText resultText = textManager::getText("devilsai", "console-damageDone");

        textManager::PlainText a;
        if (Diplomatie == DIPLOM_ALLIE)
            a += "@c[128,255,128]";
        if (Diplomatie == DIPLOM_ENNEMI)
            a += "@c[255,128,128]";
        a += displayedName();

        textManager::PlainText b;
        if (enemy->Diplomatie == DIPLOM_ALLIE)
            b += "@c[128,255,128]";
        if (enemy->Diplomatie == DIPLOM_ENNEMI)
            b += "@c[255,128,128]";
        b += enemy->displayedName();

        resultText.addParameter(a);
        resultText.addParameter((int)Degats);
        resultText.addParameter(b);

        addConsoleEntry(resultText);
        if (this == gamedata::player())
		{
            gamedata::player()->ApplicationAmeliorations();
		}
		if (enemy == gamedata::player())
		{
            if (gamedata::player()->_currentSkill->Id == enemy->behavior(Behaviors::Hurt)) gamedata::player()->BlessuresMultiples(this);
		}

        if (Degats) enemy->Set_Activite(enemy->behavior(Behaviors::Hurt));
	}
    else if (Id == gamedata::player()->Id) addConsoleEntry(textManager::getText("devilsai", "ECHEC"));
}
