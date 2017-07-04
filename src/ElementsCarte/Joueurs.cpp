
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

#include <lua5.2/lua.hpp>

#include <cmath>

#include "../Bibliotheque/Bibliotheque.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "tools/textManager.h"
#include "imageManager/imageManager.h"

#include "gamedata.h"

/** FONCTIONS DE LA CLASSE Joueur **/

Joueur::Joueur() : Individu_Unique()
{
}

string Joueur::TabToAct(int TabAppui)
{
	if (TabAppui == 0)		return PAUSE;
	if (TabAppui < 10000)	return COURSE;

	for (int c = 0 ; c < NOMBRE_COMPETENCES ; ++c)
        if (TabAppui/10000 == c+1 && skillLinks[c] != nullptr) return getStringFromLUA(skillLinks[c], "getActivite");

	return PAUSE;
}

void Joueur::Repos()
{
	Set_Vitalite(1000);
	Set_Energie(1000);
	DureeEveil = 0;
}

void Joueur::Gestion_Equipement()
{
	//Diminue la durée de vie des objets utilisés
	for (mapObjects::iterator i = Get_Caracs()->objects.objects.begin() ; i != Get_Caracs()->objects.objects.end() ; ++i)
	{
		if (getStringFromLUA(i->second, "getIdEmplacement") == i->first && getDoubleFromLUA(i->second, "getDuree") > 0)
		{
			setDoubleToLUA(i->second, "setDuree", getDoubleFromLUA(i->second, "getDuree") - tools::timeManager::I(1));

			if (getDoubleFromLUA(i->second, "getDuree") <= 0)
			{
				lua_State *j = i->second;
				i = Get_Caracs()->objects.objects.erase(i);
				Get_Caracs()->objects.deleteObject(j);
				continue;
			}
			if (getStringFromLUA(i->second, "getCategorieObjet") == "amelioratif")
			{
				if (Get_Caracs()->objects.objects.find("regulier-" + getStringFromLUA(i->second, "getTypeObject")) == Get_Caracs()->objects.objects.end())
				{
					//L'équipement amélioré n'est plus présent ; on efface l'amélioration.
					lua_State *j = i->second;
					i = Get_Caracs()->objects.objects.erase(i);
					Get_Caracs()->objects.deleteObject(j);
					continue;
				}
			}
		}
	}
}

void Joueur::Gestion_Statistiques()
{
	//1. Régénération de la vitalité
	if (get("Vitalite") < 1000)
	{
		Lag_Vitalite(tools::timeManager::I(1/4.f*tan(get("Recuperation")/70.f)));
		if (abs(get("Recuperation")) <= 95) Lag_Energie(-abs(tools::timeManager::I(get("Recuperation")/25.f*(1000-get("Vitalite"))/1000.f)));
	}

	//2. Régénération ou Perte d'Énergie lors d'une Récupération forcée
	if (abs(get("Recuperation")) > 95 && get("Energie") < ToSegment(get("Constitution"), 0, 100))
	{
		if (get("Recuperation") > 0) Lag_Energie(tools::timeManager::I(0.5));
		else Lag_Energie(-tools::timeManager::I(0.25));
	}

	//3. Perte d'énergie selon durée depuis repos
	if (DureeEveil > get("Constitution")) Lag_Energie(-tools::timeManager::I(DureeEveil-get("Constitution"))/10000);

	//4. Gain & Perte d'énergie par activité
	if (Get_Act() == PAUSE)
		Lag_Energie(tools::timeManager::I(get("Vitalite")/25000) + (LieuVillage == "village")*tools::timeManager::I(get("Vitalite")/10000));
	if (Get_Act() == COURSE)
		Lag_Energie(-tools::timeManager::I(0.05/get("Constitution")));

	//5. Évolution du taux de récupération
	float RecupCible = get("RecuperationMoyenne") * ((get("Vitalite")-200)/740.f);
	if (RecupCible > 0) RecupCible *= max(0.f, (get("Energie")-70.f)/1000.f);

	if (1.05*get("Recuperation") < RecupCible) Lag_Recuperation(tools::timeManager::I(0.1));
	if (0.95*get("Recuperation") > RecupCible) Lag_Recuperation(-tools::timeManager::I(0.1));

	if (abs(get("RecuperationMoyenne")) >= 95) Set_Recuperation(get("RecuperationMoyenne"));

	//6. Durée d'éveil
	DureeEveil += tools::timeManager::I(0.0005);

	//7. Fatigue extrême
	if (get("Energie") < 70 && get("Energie") > 10)
	{
		//Agilite-
		if (Caracs["Agilite"] > 1 && !(rand()%(int)(get("Energie")*100)+1))
			--BufAgilite;

		//Intelligence-
		if (Caracs["Intelligence"] > 1 && !(rand()%(int)(get("Energie")*100)+1))
			--BufIntelligence;

		//Application
		if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_FATIGUE), Color(255, 128, 128, 255));
			Disp_Information(tools::textManager::getText("devilsai", "FATIGUE"), true);
	}
}

void Joueur::CoupCritique(Individu* ennemi)
{
	//Agilité+, selon Agilité de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Agilite") - ennemi->get("Agilite"), 0, 10) + 1)))
		BufAgilite += ToSegment(1.1*ennemi->get("Agilite")/get("Agilite"), 0, 10);

	//Charisme+, selon Charisme de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Charisme") - ennemi->get("Charisme"), 0, 10) + 1)))
		BufCharisme += ToSegment(1.1*ennemi->get("Charisme")/get("Charisme"), 0, 10);

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_CRITIQUE), Color(128, 255, 128, 255));
		Disp_Information(tools::textManager::getText("devilsai", "CRITIQUE"), true);
}

void Joueur::BlessureGrave(Individu* ennemi)
{
	//Constitution-, selon Puissance de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Constitution") - ennemi->get("Puissance"), 0, 10) + 1)))
		BufConstitution -= min(2.0, ToSegment(1.1*ennemi->get("Puissance")/get("Constitution"), 0, 10));

	//Charisme-, selon Charisme de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Charisme") - ennemi->get("Charisme"), 0, 10) + 1)))
		BufCharisme -= min(2.0, ToSegment(1.1*ennemi->get("Charisme")/get("Charisme"), 0, 10));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_BLESSURE), Color(255, 128, 128, 255));
		Disp_Information(tools::textManager::getText("devilsai", "BLESSURE"), true);
}

void Joueur::CoupEsquive(Individu* ennemi)
{
	//Esquive+, selon Agilité de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Esquive") - ennemi->get("Agilite"), 0, 10) + 1)))
		BufEsquive += ToSegment(1.1*ennemi->get("Agilite")/get("Esquive"), 0, 3);

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_ESQUIVE), Color(128, 255, 128, 255));
		Disp_Information(tools::textManager::getText("devilsai", "ESQUIVE"), true);
}

void Joueur::GainExperience(Individu* ennemi, float Degats, int Exp)
{
	//Gain d'expérience
	if (ennemi != NULL)
	{
		if (ennemi->get("Vitalite") > 0) Exp += Degats/5000.*ennemi->Get_Experience();
		else Exp += ennemi->Get_Experience();

		//Intelligence+, Force+, Puissance+, et Constitution+, si peu fatigué
		for (int i = 0 ; i < 1 + Exp/100 ; ++i)
		{
			if (!(rand()%(int)(50 + ToSegment(-get("Energie")/100, 0, 30) + 1)))
			{
				short carac = rand()%4;
				switch(carac)
				{
					case 0 : BufIntelligence += ToSegment(2*get("Intelligence"), 0, 100)/100; break;
					case 1 : BufForce += ToSegment(10*get("Intelligence"), 0, 100)/100; break;
					case 2 : BufPuissance += ToSegment(10*get("Intelligence"), 0, 100)/100; break;
					case 3 : BufConstitution += ToSegment(5*get("Intelligence"), 0, 100)/100; break;
				}
			}
		}

		//Application
		ApplicationAmeliorations();
	}

	Experience += Exp;
}

void Joueur::BlessuresMultiples(Individu* ennemi)
{
	//Force-, selon sa propre Constitution et Puissance de l'un des ennemis
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Constitution") - ennemi->get("Puissance"), 0, 10) + 1)))
		BufForce -= min(2.0, ToSegment(1.1*ennemi->get("Puissance")/get("Constitution"), 0, 10));

	//Puissance-, selon sa propre Constitution et Puissance de l'un des ennemis
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Constitution") - ennemi->get("Puissance"), 0, 10) + 1)))
		BufPuissance -= min(2.0, ToSegment(1.1*ennemi->get("Puissance")/get("Constitution"), 0, 10));

	//Esquive-, selon sa propre Constitution et Puissance de l'un des ennemis
	if (!(rand()%(int)(5 + 10 + ToSegment(get("Constitution") - ennemi->get("Puissance"), 0, 10) + 1)))
		BufEsquive -= min(2.0, ToSegment(1.1*ennemi->get("Puissance")/get("Constitution"), 0, 10));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_BLESSURE), Color(255, 128, 128, 255));
		Disp_Information(tools::textManager::getText("devilsai", "BLESSURE"), true);
}

bool Joueur::ApplicationAmeliorations()
{
	//Affichage & enregistrement des améliorations proposées :

	String32 Final;
	Color Positif(128, 255, 128, 255); Color Negatif(255, 128, 128, 255);
	bool Retour = false;

	if ((int)BufForce != 0)
	{
		Final = tools::textManager::getFormattedText("devilsai", "AMELIORATION_FORCE", (int)BufForce);
		(BufForce > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Force"] += (int)BufForce; BufForce -= (int)BufForce; Retour = true;
	}
	if ((int)BufPuissance != 0)
	{
		Final = tools::textManager::getFormattedText("devilsai", "AMELIORATION_PUISS", (int)BufPuissance);
		(BufPuissance > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Puissance"] += (int)BufPuissance; BufPuissance -= (int)BufPuissance; Retour = true;
	}
	if ((int)BufAgilite != 0)
	{
		Final = tools::textManager::getFormattedText("devilsai", "AMELIORATION_AGILITE", (int)BufAgilite);
		(BufAgilite > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Agilite"] += (int)BufAgilite; BufAgilite -= (int)BufAgilite; Retour = true;
	}
	if ((int)BufIntelligence != 0)
	{
		Final = tools::textManager::getFormattedText("devilsai", "AMELIORATION_INTELLI", (int)BufIntelligence);
		(BufIntelligence > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Intelligence"] += (int)BufIntelligence; BufIntelligence -= (int)BufIntelligence; Retour = true;
	}
	if ((int)BufConstitution != 0)
	{
		Final = tools::textManager::getFormattedText("devilsai", "AMELIORATION_CONSTIT", (int)BufConstitution);
		(BufConstitution > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Constitution"] += (int)BufConstitution; BufConstitution -= (int)BufConstitution; Retour = true;
	}
	if ((int)BufEsquive != 0)
	{
		Final = tools::textManager::getFormattedText("devilsai", "AMELIORATION_ESQUIVE", (int)BufEsquive);
		(BufEsquive > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Esquive"] += (int)BufEsquive; BufEsquive -= (int)BufEsquive; Retour = true;
	}
	if ((int)BufCharisme != 0)
	{
		Final = tools::textManager::getFormattedText("devilsai", "AMELIORATION_CHARISM", (int)BufCharisme);
		(BufCharisme > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
		Caracs["Charisme"] += (int)BufCharisme; BufCharisme -= (int)BufCharisme; Retour = true;
	}

	return Retour;
}

void Disp_Personnage()
{
	Disp_Texte(gamedata::player()->Nom, 50, Options.ScreenH - 220, Color(255, 220, 220, 255), 35., Jeu.DayRoman);

	Disp_Texte(tools::textManager::getText("devilsai", "PERSO_VITALITE"), 50, Options.ScreenH - 170, Color(255, 255, 255, 255), 12.);
	Disp_Texte(intToString((int)gamedata::player()->get("Vitalite")), 180, Options.ScreenH - 170, Color(255, 64, 64, 255), 12.);

	Disp_Texte(tools::textManager::getText("devilsai", "PERSO_ENERGIE"), 50, Options.ScreenH - 155, Color(255, 255, 255, 255), 12.);
	Disp_Texte(intToString((int)gamedata::player()->get("Energie")), 180, Options.ScreenH - 155, Color(64, 160, 255, 255), 12.);

	Disp_Texte(tools::textManager::getText("devilsai", "PERSO_RECUP"), 50, Options.ScreenH - 140, Color(255, 255, 255, 255), 12.);
	if (gamedata::player()->get("Recuperation") >= 0)
		Disp_Texte(intToString((int)gamedata::player()->get("Recuperation")), 180, Options.ScreenH - 140, Color(64, 255, 64, 255), 12.);
	if (gamedata::player()->get("Recuperation") < 0)
		Disp_Texte(intToString((int)gamedata::player()->get("Recuperation")), 180, Options.ScreenH - 140, Color(255, 64, 255, 255), 12.);

	Disp_Texte(tools::textManager::getText("devilsai", "PERSO_EXP"), 50, Options.ScreenH - 125, Color(255, 255, 255, 255), 12.);
	Disp_Texte(intToString((int)gamedata::player()->Experience), 180, Options.ScreenH - 125, Color(255, 255, 255, 255), 12.);

	//Affichage des dégâts de la compétence Arme :
	lua_State* weapon = NULL;
/*	for (int i = 0 ; i < NOMBRE_COMPETENCES ; ++i)
		if (Partie.perso->CompetencesRapides[i] != NULL && Partie.perso->CompetencesRapides[i]->Id/100 == TYPE_COMP_ARME)
 */ //			cpt_arme = dynamic_cast<Competence_Arme*>(Partie.perso->CompetencesRapides[/*i*/COMPETENCE_CTRL]);

	weapon = gamedata::player()->skillLinks[COMPETENCE_CTRL];

	if (weapon != NULL)
	{
		Disp_Texte(tools::textManager::getText("devilsai", "PERSO_DEGATS"), 50, Options.ScreenH - 110, Color(255, 255, 255, 255), 12.);
		string StrNombre = intToString(getDoubleFromLUA(weapon, "getDegats") - getDoubleFromLUA(weapon, "getAmplitude") + (int)gamedata::player()->get("Force")/2)
						+ " - " + intToString(getDoubleFromLUA(weapon, "getDegats") + getDoubleFromLUA(weapon, "getAmplitude") + (int)gamedata::player()->get("Force")/2);
		Disp_Texte(StrNombre, 180, Options.ScreenH - 110, Color(255, 255, 255, 255), 12.);
	}

    Disp_Texte(tools::textManager::getText("devilsai", "PERSO_FORCE"), 280, Options.ScreenH - 180, Color(255, 255, 255, 255), 12.);
    Disp_Texte(tools::textManager::getText("devilsai", "PERSO_PUISS"), 280, Options.ScreenH - 180 + 15, Color(255, 255, 255, 255), 12.);
    Disp_Texte(tools::textManager::getText("devilsai", "PERSO_AGILITE"), 280, Options.ScreenH - 180 + 30, Color(255, 255, 255, 255), 12.);
    Disp_Texte(tools::textManager::getText("devilsai", "PERSO_INTELLI"), 280, Options.ScreenH - 180 + 45, Color(255, 255, 255, 255), 12.);
    Disp_Texte(tools::textManager::getText("devilsai", "PERSO_CONSTIT"), 280, Options.ScreenH - 180 + 60, Color(255, 255, 255, 255), 12.);
    Disp_Texte(tools::textManager::getText("devilsai", "PERSO_ESQUIVE"), 280, Options.ScreenH - 180 + 75, Color(255, 255, 255, 255), 12.);
    Disp_Texte(tools::textManager::getText("devilsai", "PERSO_CHARISM"), 280, Options.ScreenH - 180 + 90, Color(255, 255, 255, 255), 12.);
    Disp_Texte(tools::textManager::getText("devilsai", "PERSO_RECUPMOY"), 280, Options.ScreenH - 180 + 105, Color(255, 255, 255, 255), 12.);

	int numberChar = 0;
	for (string characteristic : {	"Force", "Puissance", "Agilite", "Intelligence", "Constitution", "Esquive", "Charisme",
									"RecuperationMoyenne"})
	{
		int absoluteQty = gamedata::player()->Caracs[characteristic];
		int diffQty = gamedata::player()->get(characteristic) - absoluteQty;

		Disp_Texte(intToString(absoluteQty), 430, Options.ScreenH - 180 + 15*numberChar, Color(255, 255, 255, 255), 12.);

		if (diffQty > 0)
			Disp_Texte("+" + intToString(diffQty), 460, Options.ScreenH - 180 + 15*numberChar, Color(64, 255, 64, 255), 12.);
		if (diffQty < 0)
			Disp_Texte(intToString(diffQty), 460, Options.ScreenH - 180 + 15*numberChar, Color(255, 64, 255, 255), 12.);

		++numberChar;
	}
}

void Disp_MiniaturesCompetences()
{
    imageManager::display(Jeu.App, "misc", "FondMiniaturesCompetences", 5, Options.ScreenH - 171);

    if (gamedata::player()->skillLinks[0] != nullptr)
        imageManager::display(Jeu.App, "skills", getStringFromLUA(gamedata::player()->skillLinks[0], "getName"), 5, Options.ScreenH - 55);

    if (gamedata::player()->skillLinks[1] != nullptr)
        imageManager::display(Jeu.App, "skills", getStringFromLUA(gamedata::player()->skillLinks[1], "getName"), 5, Options.ScreenH - 105);

    if (gamedata::player()->skillLinks[2] != nullptr)
        imageManager::display(Jeu.App, "skills", getStringFromLUA(gamedata::player()->skillLinks[2], "getName"), 5, Options.ScreenH - 171);

    if (gamedata::player()->skillLinks[3] != nullptr)
        imageManager::display(Jeu.App, "skills", getStringFromLUA(gamedata::player()->skillLinks[3], "getName"), 71, Options.ScreenH - 55);
}
