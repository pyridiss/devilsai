
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

#include "textManager/richText.h"

#include "../Bibliotheque/Bibliotheque.h"
#include "Attributs/Attributs.h"

void Disp_Caracs_Objet(RenderWindow& target, lua_State* obj)
{
    textManager::PlainText description;

    string internalNumber = getStringFromLUA(obj, "getFileName");

    description += "@f[dayroman]@s[20]@c[255,220,255]";
    description += textManager::getText("objects", internalNumber);
    description += " @d";

    if (getBoolFromLUA(obj, "getDescriptionManuelle"))
	{
        description += " @n";
        description += textManager::getText("objects", internalNumber + "-description");
	}
	if (getDoubleFromLUA(obj, "getDuree") > 0)
	{
        description += " @n";
        description += textManager::getText("devilsai", "EQUIP_DUREE");
        description.addParameter((int)(getDoubleFromLUA(obj, "getDuree")/60.f));
	}

    if (getBoolFromLUA(obj, "getDescriptionAutomatique"))
    {
        for (auto& p : AttributesNames)
        {
            lua_getglobal(obj, "getObjectProperty");
            lua_pushstring(obj, p);
            lua_call(obj, 1, 1);
            int value = lua_tonumber(obj, -1);
            if (value != 0)
            {
                description += " @n";
                description += textManager::getText("devilsai", string("object-property-") + p);
                description.addParameter(value);
            }
        }
        for (auto& p : AttributesAmplifiersNames)
        {
            lua_getglobal(obj, "getObjectProperty");
            lua_pushstring(obj, p);
            lua_call(obj, 1, 1);
            int value = lua_tonumber(obj, -1);
            if (value != 0)
            {
                description += " @n";
                description += textManager::getText("devilsai", string("object-property-") + p);
                description.addParameter(value);
            }
        }
    }

    textManager::RichText rich;
    rich.setSize(300, 0);
    rich.setDefaultProperties("liberation", 12, Color(255, 255, 255));
    rich.addFlags(textManager::HAlignCenter);
    rich.create(description);

    float x = gui::mousePosition().x;
    float y = gui::mousePosition().y;
    float w = 300;
    float h = rich.height();

    if (y + h > target.getSize().y)
        y -= h;

    gui::style::textBackgroundShader(target, x - 10, y - 10, w + 2*10, h + 2*10);

    rich.displayFullText(target, x, y);
}
