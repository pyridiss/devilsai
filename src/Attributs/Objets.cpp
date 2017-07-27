
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
#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"

#include "tools/textManager.h"

#include "imageManager/imageManager.h"
#include "gui/button.h"

#include "gamedata.h"

gui::Button *BoutonsInventaire[24];
gui::Button *BoutonsCoffre[8];
gui::Button *BoutonsCompetences[4]; //Never displayed, but useful to test activation

int PosDescX = 0, PosDescY = 0;
int PosCoffreX = 0, PosCoffreY = 0;

void Load_Decorations_Objets()
{
    for (int a = 0 ; a < 24 ; ++a)
    {
        BoutonsInventaire[a] = new gui::Button;
        BoutonsInventaire[a]->setTopLeftCoordinates(150 + 5 + 50*(a%12), Options.ScreenH - 110 + 50*(a/12));
        BoutonsInventaire[a]->setSize(50, 50);
        BoutonsInventaire[a]->setAllBackground("FondObjet_50_50");
        BoutonsInventaire[a]->setForegroundShader("hover", gui::style::highlightShader);
    }

	if (Options.ScreenW > 150 + 610 + 250) //Assez de place à droite de l'inventaire
	{
		PosCoffreX = 790; PosCoffreY = Options.ScreenH - 130;
		PosDescX = Options.ScreenW - 145; PosDescY = Options.ScreenH - 290;
	}
	else
	{
		PosCoffreX = 550; PosCoffreY = Options.ScreenH - 260;
		PosDescX = Options.ScreenW - 145; PosDescY = 100;
	}

    for (int a = 0 ; a < 8 ; ++a)
    {
        BoutonsCoffre[a] = new gui::Button;
        BoutonsCoffre[a]->setTopLeftCoordinates(PosCoffreX + 5 + 50*(a%4), PosCoffreY + 20 + 50*(a/4));
        BoutonsCoffre[a]->setSize(50, 50);
        BoutonsCoffre[a]->setAllBackground("FondObjet_50_50");
        BoutonsCoffre[a]->setForegroundShader("hover", gui::style::highlightShader);
    }

    unsigned yCompetences[] = {Options.ScreenH - 55, Options.ScreenH - 105, Options.ScreenH - 171, Options.ScreenH - 55};
    for (int a = 0 ; a < 4 ; ++a)
    {
        BoutonsCompetences[a] = new gui::Button;
        BoutonsCompetences[a]->setTopLeftCoordinates(5 + 66*(a/3), yCompetences[a]);
        BoutonsCompetences[a]->setSize(50, 50);
        BoutonsCompetences[a]->setAllBackground("FondObjet_50_50");
        BoutonsCompetences[a]->setForegroundShader("hover", gui::style::highlightShader);
    }
}

void Supprimer_Decorations_Objets()
{
	for (int a = 0 ; a < 24 ; ++a)
		delete BoutonsInventaire[a];

	for (int a = 0 ; a < 8 ; ++a)
		delete BoutonsCoffre[a];

	for (int a = 0 ; a < 4 ; ++a)
		delete BoutonsCompetences[a];
}

void Disp_Skill(lua_State* skill)
{
	static int LigneCourante = PosDescY;

	string internalNumber = getStringFromLUA(skill, "getInternalNumber");

	String32 nom = getTranslatedNameOfSkill(internalNumber);
	Disp_TexteCentre(nom, PosDescX, LigneCourante, Color(255, 220, 220, 255), 20., Jeu.DayRoman);
}

void Disp_Competences()
{
	imageManager::display(Jeu.App, "misc", "FondInventaire", 150, Options.ScreenH - 130);
	Disp_TexteCentre(tools::textManager::getText("devilsai", "COMPETENCES"), 455, Options.ScreenH - 120, Color(255, 255, 255, 255), 13.);

	mapSkills::iterator skill = gamedata::player()->Get_Caracs()->skills.begin();

	for (int cmpt = 0 ; cmpt < 24 ; ++cmpt)
	{
        BoutonsInventaire[cmpt]->display(Jeu.App);
		if (skill != gamedata::player()->Get_Caracs()->skills.end())
		{
            string name = getStringFromLUA(skill->second, "getName");
            imageManager::display(Jeu.App, "skills", name, BoutonsInventaire[cmpt]->getXTopLeft(), BoutonsInventaire[cmpt]->getYTopLeft());
            if (BoutonsInventaire[cmpt]->mouseHovering(Jeu.App))
                Disp_Skill(skill->second);

			++skill;
		}
	}

	Disp_MiniaturesCompetences();

	if (Partie.selectedSkill != nullptr)
        imageManager::display(Jeu.App, "skills", getStringFromLUA(Partie.selectedSkill, "getName"), Mouse::getPosition(Jeu.App).x, Mouse::getPosition(Jeu.App).y);
}

void Gestion_Competences(Event &event)
{
	mapSkills::iterator skill = gamedata::player()->Get_Caracs()->skills.begin();

	for (int cmpt = 0 ; cmpt < 24 ; ++cmpt)
	{
		if (skill != gamedata::player()->Get_Caracs()->skills.end())
		{
            if (BoutonsInventaire[cmpt]->activated(Jeu.App, event))
                Partie.selectedSkill = skill->second;

			++skill;
		}
	}

	if (Partie.selectedSkill != nullptr)
	{
		for (int i : {COMPETENCE_CTRL, COMPETENCE_SHIFT, COMPETENCE_TAB, COMPETENCE_SPACE})
		if (BoutonsCompetences[i]->activated(Jeu.App, event))
		{
			gamedata::player()->skillLinks[i] = Partie.selectedSkill;
			//We must remove duplicates of this skill
			for (int j : {COMPETENCE_CTRL, COMPETENCE_SHIFT, COMPETENCE_TAB, COMPETENCE_SPACE})
			{
				if (i != j && gamedata::player()->skillLinks[j] == Partie.selectedSkill)
                    gamedata::player()->skillLinks[j] = nullptr;
			}
			Partie.selectedSkill = nullptr;
		}
	}
}

void Disp_EmplacementVide(string TypeObjet)
{
	Disp_TexteCentre(tools::textManager::getFormattedText("devilsai", "EQUIP_VIDE", getTranslatedNameOfObject(TypeObjet)), PosDescX, PosDescY, Color(255, 220, 220, 255), 20., Jeu.DayRoman);
}

void Disp_Caracs_Objet(lua_State* obj, bool MaJ)
{
	static int LigneCourante = PosDescY;
	if (MaJ) LigneCourante = PosDescY;

	int internalNumber = getIntFromLUA(obj, "getInternalNumber");

	String32 nom = getTranslatedNameOfObject(internalNumber);
	Disp_TexteCentre(nom, PosDescX, LigneCourante, Color(255, 220, 220, 255), 20., Jeu.DayRoman);

	LigneCourante += 26;

	if (getBoolFromLUA(obj, "getDescriptionManuelle") && MaJ)
	{
		//Pour ne pas surcharger, on désactive les descriptions manuelles des objets supplémentaires
		Paragraph replique;
		replique.characters = getTranslatedDescriptionOfObject(internalNumber);
		replique.rectangle = IntRect(0, 0, 2*(Options.ScreenW - PosDescX) - 20, 0);
		cutParagraph(&replique);
		for (ListString32::iterator i = replique.lines.begin() ; i != replique.lines.end() ; ++i)
		{
			Disp_TexteCentre(*i, PosDescX, LigneCourante-4, Color(255, 220, 220), 12.);
			LigneCourante += 14;
		}
	}
	if (getDoubleFromLUA(obj, "getDuree") > 0)
	{
		Disp_TexteCentre(tools::textManager::getFormattedText("devilsai", "EQUIP_DUREE", (int)(getDoubleFromLUA(obj, "getDuree")/60.f)), PosDescX, LigneCourante-4, Color(255, 220, 220), 12.);
		LigneCourante += 16;
	}

	if (getBoolFromLUA(obj, "getDescriptionAutomatique")) for (int Carac = 0 ; Carac < 19 ; ++Carac)
	{
		string phrase; string functionLUA = "";

		switch(Carac)
		{
			case 0 :	functionLUA = "getAbsoluteForce";				phrase = "EQUIP_FORCE";			break;
			case 1 :	functionLUA = "getAbsoluteMultForce";			phrase = "EQUIP_MULT_FORCE";		break;
			case 2 :	functionLUA = "getAbsolutePuissance";			phrase = "EQUIP_PUISS";			break;
			case 3 :	functionLUA = "getAbsoluteMultPuissance";		phrase = "EQUIP_MULT_PUISS";		break;
			case 4 :	functionLUA = "getAbsoluteAgilite";				phrase = "EQUIP_AGILITE";		break;
			case 5 :	functionLUA = "getAbsoluteMultAgilite";			phrase = "EQUIP_MULT_AGILITE";	break;
			case 6 :	functionLUA = "getAbsoluteIntelligence";		phrase = "EQUIP_INTELLI";		break;
			case 7 :	functionLUA = "getAbsoluteMultIntelligence";	phrase = "EQUIP_MULT_INTELLI";	break;
			case 8 :	functionLUA = "getAbsoluteConstitution";		phrase = "EQUIP_CONSTIT";		break;
			case 9 :	functionLUA = "getAbsoluteMultConstitution";	phrase = "EQUIP_MULT_CONSTIT";	break;
			case 10 :	functionLUA = "getAbsoluteEsquive";				phrase = "EQUIP_ESQUIVE";		break;
			case 11 :	functionLUA = "getAbsoluteMultEsquive";			phrase = "EQUIP_MULT_ESQUIVE";	break;
			case 12 :	functionLUA = "getAbsoluteCharisme";			phrase = "EQUIP_CHARISM";		break;
			case 13 :	functionLUA = "getAbsoluteMultCharisme";		phrase = "EQUIP_MULT_CHARISM";	break;
			case 14 :	functionLUA = "getAbsoluteRecuperationMoyenne";		phrase = "EQUIP_RECUP";			break;
			case 15 :	functionLUA = "getAbsoluteMultRecuperationMoyenne";	phrase = "EQUIP_MULT_RECUP";		break;
			case 16 :	functionLUA = "getAbsoluteVitesseCourse";		phrase = "EQUIP_VIT_COURSE";		break;
			case 17 :	functionLUA = "getAbsoluteVitesseAttaque";		phrase = "EQUIP_VIT_ATTAQUE";	break;
			case 18 :	functionLUA = "getAbsoluteVitesseBlesse";		phrase = "EQUIP_VIT_BLESSE";		break;
		}

		int value = getIntFromLUA(obj, functionLUA);
		if (value != 0)
		{
			Disp_TexteCentre(tools::textManager::getFormattedText("devilsai", phrase, value), PosDescX, LigneCourante, Color(255, 255, 255), 11.);
			LigneCourante += 14;
		}
	}
	LigneCourante += 26;
}
