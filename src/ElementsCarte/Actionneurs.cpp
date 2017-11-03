
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
#include <lua.hpp>

#include "tools/filesystem.h"

#include "Bibliotheque/luaFunctions.h"

#include "gamedata.h"
#include "Jeu/options.h"

#include "ElementsCarte.h"

using namespace tinyxml2;


CheckPoint::CheckPoint() : Element_Carte()
{
    inert = true;
    ignoreCollision = true;
}

int CheckPoint::Gestion()
{
	return ETAT_NORMAL;
}

int CheckPoint::Collision(Individu* elem, int TypeCollision)
{
	return COLL_OK;
}

void CheckPoint::loadFromXML(tinyxml2::XMLHandle &handle)
{
}

void CheckPoint::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
}

void CheckPoint::Disp(RenderTarget& target)
{
    if (options::option<bool>("displayShapes"))
        size.display(target, Color(255, 0, 0, 50));
	return;
}


/** FONCTIONS DE LA CLASSE Trigger **/

Trigger::Trigger() : Element_Carte()
{
	Element_Carte::Type = "ACTIONNEUR";
    inert = true;
}
Trigger::~Trigger()
{
}

int Trigger::Gestion()
{
    return ETAT_NORMAL;
}

int Trigger::Collision(Individu* elem, int TypeCollision)
{
    if (elem->Type == "intern")
    {
        lua_getglobal(script, "mouseHovering");
        lua_pushstring(script, data.c_str());
        lua_call(script, 1, 0);
        return 0;
    }

    lua_getglobal(script, "collision");
    lua_pushlightuserdata(script, (void*)elem);
    lua_pushnumber(script, TypeCollision);
    lua_pushstring(script, data.c_str());
    lua_call(script, 3, 1);

    int value = lua_tonumber(script, -1);

    return value;
}

void Trigger::loadFromXML(tinyxml2::XMLHandle &handle)
{
    XMLElement *elem = handle.ToElement();

    if (elem->Attribute("script"))
    {
        triggerScript = elem->Attribute("script");
        script = gamedata::sharedTrigger(triggerScript);
    }

    double x = 0, y = 0;
    elem->QueryAttribute("x", &x);
    elem->QueryAttribute("y", &y);
    move(x, y);

    if (elem->Attribute("tag"))
        Liste = elem->Attribute("tag");

    if (elem->Attribute("data"))
        data = elem->Attribute("data");

    elem = handle.FirstChildElement().ToElement();
    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "shape")
            size.loadFromXML(elem);

        elem = elem->NextSiblingElement();
    }
}

void Trigger::saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* trigger = doc.NewElement("trigger");
    trigger->SetAttribute("script", triggerScript.c_str());
    trigger->SetAttribute("x", position().x);
    trigger->SetAttribute("y", position().y);
    trigger->SetAttribute("tag", Liste.c_str());
    trigger->SetAttribute("data", data.c_str());

    XMLElement* shape = doc.NewElement("shape");
    trigger->InsertEndChild(shape);
    XMLHandle shapeHandle(shape);
    size.saveToXML(doc, shapeHandle);

    root->InsertEndChild(trigger);
}

void Trigger::Disp(RenderTarget& target)
{
    if (options::option<bool>("displayShapes"))
        size.display(target, Color(0, 255, 0, 50));
	return;
}
