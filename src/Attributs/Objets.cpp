
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

#include <lua.hpp>

#include "../Bibliotheque/Bibliotheque.h"
#include "../Jeu/Jeu.h"
#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"

#include "tools/textManager.h"

#include "imageManager/imageManager.h"
#include "gui/button.h"

#include "gamedata.h"

int PosDescX = 0, PosDescY = 0;

void Load_Decorations_Objets()
{

	if (Options.ScreenW > 150 + 610 + 250) //Assez de place à droite de l'inventaire
	{
		PosDescX = Options.ScreenW - 145; PosDescY = Options.ScreenH - 290;
	}
	else
	{
		PosDescX = Options.ScreenW - 145; PosDescY = 100;
	}
}

void Disp_Skill(lua_State* skill)
{
	static int LigneCourante = PosDescY;

	string internalNumber = getStringFromLUA(skill, "getInternalNumber");

	String32 nom = getTranslatedNameOfSkill(internalNumber);
	Disp_TexteCentre(nom, PosDescX, LigneCourante, Color(255, 220, 220, 255), 20., Jeu.DayRoman);
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

    if (getBoolFromLUA(obj, "getDescriptionAutomatique"))
    {
        for (auto& p : ObjectProperties)
        {
            lua_getglobal(obj, "getObjectProperty");
            lua_pushstring(obj, p.c_str());
            lua_call(obj, 1, 1);
            int value = lua_tonumber(obj, -1);
            if (value != 0)
            {
                Disp_TexteCentre(tools::textManager::getFormattedText("devilsai", "object-property-" + p, value), PosDescX, LigneCourante, Color(255, 255, 255), 11.);
                LigneCourante += 14;
            }
        }
    }

	LigneCourante += 26;
}
