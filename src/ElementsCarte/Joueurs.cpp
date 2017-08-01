
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

#include <tinyxml2.h>
#include <lua5.2/lua.hpp>

#include <cmath>

#include "../Bibliotheque/Bibliotheque.h"
#include "../Jeu/Jeu.h"
#include "ElementsCarte.h"

#include "tools/timeManager.h"
#include "tools/textManager.h"
#include "imageManager/imageManager.h"

#include "gamedata.h"

using namespace tinyxml2;

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
	setHealthStatus(Statistiques::Life, 1000);
	setHealthStatus(Statistiques::Energy, 1000);
	DureeEveil = 0;
}

void Joueur::Gestion_Statistiques()
{
	//3. Perte d'énergie selon durée depuis repos
	if (DureeEveil > currentHealthStatus(Caracteristiques::Constitution)) modifyHealthStatus(Statistiques::Energy, -tools::timeManager::I(DureeEveil-currentHealthStatus(Caracteristiques::Constitution))/10000);

	//4. Gain & Perte d'énergie par activité
	if (Get_Act() == PAUSE)
		modifyHealthStatus(Statistiques::Energy, tools::timeManager::I(currentHealthStatus(Statistiques::Life)/25000) /*+ (LieuVillage == "village")*tools::timeManager::I(currentHealthStatus(Statistiques::Life)/10000)*/);
	if (Get_Act() == COURSE)
		modifyHealthStatus(Statistiques::Energy, -tools::timeManager::I(0.05/currentHealthStatus(Caracteristiques::Constitution)));

	//6. Durée d'éveil
	DureeEveil += tools::timeManager::I(0.0005);

	//7. Fatigue extrême
	if (currentHealthStatus(Statistiques::Energy) < 70 && currentHealthStatus(Statistiques::Energy) > 10)
	{
		//Agilite-
        if (Caracs[Caracteristiques::Agility] > 1)
            attributes().add(Caracteristiques::Agility, -(70.0-currentHealthStatus(Statistiques::Energy))/100000.0*tools::timeManager::I(1));

		//Intelligence-
        if (Caracs[Caracteristiques::Intellect] > 1)
            attributes().add(Caracteristiques::Intellect, -(70.0-currentHealthStatus(Statistiques::Energy))/100000.0*tools::timeManager::I(1));

		//Application
		if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_FATIGUE), Color(255, 128, 128, 255));
			Disp_Information(tools::textManager::getText("devilsai", "FATIGUE"), true);
	}
}

void Joueur::CoupCritique(Individu* ennemi)
{
	//Agilité+, selon Agilité de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Caracteristiques::Agility) - ennemi->currentHealthStatus(Caracteristiques::Agility), 0, 10) + 1)))
        attributes().add(Caracteristiques::Agility, ToSegment(1.1*ennemi->currentHealthStatus(Caracteristiques::Agility)/currentHealthStatus(Caracteristiques::Agility), 0, 10));

	//Charisme+, selon Charisme de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Caracteristiques::Charisma) - ennemi->currentHealthStatus(Caracteristiques::Charisma), 0, 10) + 1)))
		attributes().add(Caracteristiques::Charisma, ToSegment(1.1*ennemi->currentHealthStatus(Caracteristiques::Charisma)/currentHealthStatus(Caracteristiques::Charisma), 0, 10));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_CRITIQUE), Color(128, 255, 128, 255));
		Disp_Information(tools::textManager::getText("devilsai", "CRITIQUE"), true);
}

void Joueur::BlessureGrave(Individu* ennemi)
{
	//Constitution-, selon Puissance de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Caracteristiques::Constitution) - ennemi->currentHealthStatus(Caracteristiques::Power), 0, 10) + 1)))
        attributes().add(Caracteristiques::Constitution, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Caracteristiques::Power)/currentHealthStatus(Caracteristiques::Constitution), 0, 10)));

	//Charisme-, selon Charisme de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Caracteristiques::Charisma) - ennemi->currentHealthStatus(Caracteristiques::Charisma), 0, 10) + 1)))
        attributes().add(Caracteristiques::Charisma, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Caracteristiques::Charisma)/currentHealthStatus(Caracteristiques::Charisma), 0, 10)));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_BLESSURE), Color(255, 128, 128, 255));
		Disp_Information(tools::textManager::getText("devilsai", "BLESSURE"), true);
}

void Joueur::CoupEsquive(Individu* ennemi)
{
	//Esquive+, selon Agilité de l'ennemi
	if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Caracteristiques::Dodge) - ennemi->currentHealthStatus(Caracteristiques::Agility), 0, 10) + 1)))
        attributes().add(Caracteristiques::Dodge, ToSegment(1.1*ennemi->currentHealthStatus(Caracteristiques::Agility)/currentHealthStatus(Caracteristiques::Dodge), 0, 3));

	//Application
	if (ApplicationAmeliorations()) //Ajouter_LigneAmelioration(Get_Phrase(_ESQUIVE), Color(128, 255, 128, 255));
		Disp_Information(tools::textManager::getText("devilsai", "ESQUIVE"), true);
}

void Joueur::GainExperience(Individu* ennemi, float Degats, int Exp)
{
	//Gain d'expérience
	if (ennemi != NULL)
	{
		if (ennemi->currentHealthStatus(Statistiques::Life) > 0) Exp += Degats/5000.*ennemi->Get_Experience();
		else Exp += ennemi->Get_Experience();

		//Intelligence+, Force+, Puissance+, et Constitution+, si peu fatigué
		for (int i = 0 ; i < 1 + Exp/100 ; ++i)
		{
			if (!(rand()%(int)(50 + ToSegment(-currentHealthStatus(Statistiques::Energy)/100, 0, 30) + 1)))
			{
				short carac = rand()%4;
				switch(carac)
				{
                    case 0 : attributes().add(Caracteristiques::Intellect, ToSegment(2*currentHealthStatus(Caracteristiques::Intellect), 0, 100)/100); break;
                    case 1 : attributes().add(Caracteristiques::Strength, ToSegment(10*currentHealthStatus(Caracteristiques::Intellect), 0, 100)/100); break;
                    case 2 : attributes().add(Caracteristiques::Power, ToSegment(10*currentHealthStatus(Caracteristiques::Intellect), 0, 100)/100); break;
                    case 3 : attributes().add(Caracteristiques::Constitution, ToSegment(5*currentHealthStatus(Caracteristiques::Intellect), 0, 100)/100); break;
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
	if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Caracteristiques::Constitution) - ennemi->currentHealthStatus(Caracteristiques::Power), 0, 10) + 1)))
        attributes().add(Caracteristiques::Strength, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Caracteristiques::Power)/currentHealthStatus(Caracteristiques::Constitution), 0, 10)));

	//Puissance-, selon sa propre Constitution et Puissance de l'un des ennemis
	if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Caracteristiques::Constitution) - ennemi->currentHealthStatus(Caracteristiques::Power), 0, 10) + 1)))
        attributes().add(Caracteristiques::Power, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Caracteristiques::Power)/currentHealthStatus(Caracteristiques::Constitution), 0, 10)));

	//Esquive-, selon sa propre Constitution et Puissance de l'un des ennemis
	if (!(rand()%(int)(5 + 10 + ToSegment(currentHealthStatus(Caracteristiques::Constitution) - ennemi->currentHealthStatus(Caracteristiques::Power), 0, 10) + 1)))
        attributes().add(Caracteristiques::Dodge, -min(2.0, ToSegment(1.1*ennemi->currentHealthStatus(Caracteristiques::Power)/currentHealthStatus(Caracteristiques::Constitution), 0, 10)));

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

    for (int c = 0 ; c != Caracteristiques::Attribute::enumSize ; ++c)
    {
        Caracteristiques::Attribute att = static_cast<Caracteristiques::Attribute>(c);
        int diff = floor(attributes()[att]) - _displayedAttributes[att];

        if (diff != 0)
        {
            string text = "console-attributeImprovement-" + Caracteristiques::toString(att);
            Final = tools::textManager::getFormattedText("devilsai", text, diff);
            (diff > 0) ? Ajouter_LigneAmelioration(Final, Positif) : Ajouter_LigneAmelioration(Final, Negatif);
        }

        _displayedAttributes.set(att, floor(attributes()[att]));
    }

	return Retour;
}

void Joueur::resetDisplayedAttributes()
{
    for (int c = 0 ; c != Caracteristiques::Attribute::enumSize ; ++c)
    {
        Caracteristiques::Attribute att = static_cast<Caracteristiques::Attribute>(c);
        _displayedAttributes.set(att, floor(attributes()[att]));
    }
}

void Joueur::loadFromXML(tinyxml2::XMLHandle &handle)
{
    Individu_Unique::loadFromXML(handle);
    resetDisplayedAttributes();
}

void Joueur::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    Individu_Unique::saveToXML(doc, handle);
}

void Disp_Personnage()
{
	Disp_Texte(gamedata::player()->Nom, 50, Options.ScreenH - 220, Color(255, 220, 220, 255), 35., Jeu.DayRoman);

	Disp_Texte(tools::textManager::getText("devilsai", "PERSO_VITALITE"), 50, Options.ScreenH - 170, Color(255, 255, 255, 255), 12.);
	Disp_Texte(intToString((int)gamedata::player()->currentHealthStatus(Statistiques::Life)), 180, Options.ScreenH - 170, Color(255, 64, 64, 255), 12.);

	Disp_Texte(tools::textManager::getText("devilsai", "PERSO_ENERGIE"), 50, Options.ScreenH - 155, Color(255, 255, 255, 255), 12.);
	Disp_Texte(intToString((int)gamedata::player()->currentHealthStatus(Statistiques::Energy)), 180, Options.ScreenH - 155, Color(64, 160, 255, 255), 12.);

	Disp_Texte(tools::textManager::getText("devilsai", "PERSO_RECUP"), 50, Options.ScreenH - 140, Color(255, 255, 255, 255), 12.);
	if (gamedata::player()->currentHealthStatus(Statistiques::Healing) >= 0)
		Disp_Texte(intToString((int)gamedata::player()->currentHealthStatus(Statistiques::Healing)), 180, Options.ScreenH - 140, Color(64, 255, 64, 255), 12.);
	if (gamedata::player()->currentHealthStatus(Statistiques::Healing) < 0)
		Disp_Texte(intToString((int)gamedata::player()->currentHealthStatus(Statistiques::Healing)), 180, Options.ScreenH - 140, Color(255, 64, 255, 255), 12.);

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
		string StrNombre = intToString(getDoubleFromLUA(weapon, "getDegats") - getDoubleFromLUA(weapon, "getAmplitude") + (int)gamedata::player()->currentHealthStatus(Caracteristiques::Strength)/2)
						+ " - " + intToString(getDoubleFromLUA(weapon, "getDegats") + getDoubleFromLUA(weapon, "getAmplitude") + (int)gamedata::player()->currentHealthStatus(Caracteristiques::Strength)/2);
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
    for (int c = 0 ; c != Caracteristiques::Attribute::RunSpeed ; ++c)
	{
        int absoluteQty = gamedata::player()->attributes()[static_cast<Caracteristiques::Attribute>(c)];
        int diffQty = gamedata::player()->currentHealthStatus(static_cast<Caracteristiques::Attribute>(c)) - absoluteQty;

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
