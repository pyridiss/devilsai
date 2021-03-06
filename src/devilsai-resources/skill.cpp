
/*
    devilsai - An Action-RPG game
    Copyright (C) 2009-2018  Quentin Henriet

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

#include "devilsai-resources/skill.h"

#include <lua.hpp>
#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/math.h"
#include "textManager/textManager.h"
#include "imageManager/imageManager.h"

#include "../Bibliotheque/luaFunctions.h"
#include "../ElementsCarte/ElementsCarte.h"

#include "devilsai-resources/Carte.h"
#include "devilsai-resources/manager.h"


using namespace tinyxml2;

Skill::Skill(string id)
  : Id(std::move(id)),
    Animation(),
    scriptString(),
    script(nullptr),
    interactionField(),
    numberOfImages(0),
    priority(0),
    step(0),
    levels(1),
    speedAttribute(RunSpeed)
{
}

Skill::Skill(const Skill& other)
  : Id(other.Id),
    Animation(other.Animation),
    scriptString(other.scriptString),
    script(nullptr),
    interactionField(other.interactionField),
    numberOfImages(other.numberOfImages),
    priority(other.priority),
    step(other.step),
    levels(other.levels),
    speedAttribute(other.speedAttribute)
{
    if (!scriptString.empty())
        loadScript();
}

Skill& Skill::operator=(const Skill& right)
{
    Id = right.Id;
    interactionField = right.interactionField;
    Animation = right.Animation;
    scriptString = right.scriptString;
    numberOfImages = right.numberOfImages;
    priority = right.priority;
    step = right.step;
    levels = right.levels;
    speedAttribute = right.speedAttribute;

    if (!scriptString.empty())
        loadScript();

    return *this;
}

Skill::~Skill()
{
    if (script != nullptr) lua_close(script);
}

void Skill::addImage(double angle, int num, string imageKey)
{
    pair<double, int> key(angle, num);
    Animation.insert(pair<pair<double,int>,string>(key, imageKey));

    if (angle == 0) addImage(2.0 * M_PI, num, imageKey);
}

string Skill::getImageKey(double angle, int num)
{
    string key = Animation.begin()->second;
    double distance = 10;

    for (auto& i : Animation)
    {
        if (i.first.second != num) continue;

        double d = (i.first.first - angle) * (i.first.first - angle);
        if (d < distance)
        {
            distance = d;
            key = i.second;
        }
    }

    return key;
}

void Skill::loadFromXML(XMLHandle &handle, Shader* shader, int hue, int saturation, int luminance)
{
    XMLElement *elem = handle.FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "properties")
        {
            elem->QueryAttribute("priority", &priority);
            elem->QueryAttribute("step", &step);
            elem->QueryAttribute("numberOfImages", &numberOfImages);

            if (elem->Attribute("speedAttribute"))
                speedAttribute = stringToAttribute(elem->Attribute("speedAttribute"));

            //Should be removed when no data file use this
            if (elem->Attribute("speedImprover"))
                speedAttribute = stringToAttribute(elem->Attribute("speedImprover"));

            if (elem->Attribute("speed"))
            {
                double sp = 0;
                elem->QueryAttribute("speed", &sp);
                levels[0].extraStats.set(speedAttribute, sp);
            }
        }
        if (elemName == "level")
        {
            unsigned level = 1;
            elem->QueryAttribute("level", &level);
            levels.resize(max(level, (unsigned)levels.size()));
            for (XMLElement *elem2 = elem->FirstChildElement() ; elem2 != nullptr ; elem2 = elem2->NextSiblingElement())
            {
                string_view elem2Name = elem2->Name();

                if (elem2Name == "extraStats")
                    levels[level-1].extraStats.loadFromXML(elem2);
                if (elem2Name == "extraStatsAmplifiers")
                    levels[level-1].extraStatsAmplifiers.loadFromXML(elem2);
                if (elem2Name == "unavailability")
                    elem2->QueryAttribute("value", &(levels[level-1].unavailability));
            }
        }
        if (elemName == "extraStats")  // When outside a 'level' element, assume (level == 1)
        {
            levels[0].extraStats.loadFromXML(elem);
        }
        if (elemName == "extraStatsAmplifiers")  // When outside a 'level' element, assume (level == 1)
        {
            levels[0].extraStatsAmplifiers.loadFromXML(elem);
        }
        if (elemName == "unavailability")  // When outside a 'level' element, assume (level == 1)
        {
            elem->QueryAttribute("value", &(levels[0].unavailability));
        }
        if (elemName == "interactionField")
        {
            interactionField.loadFromXML(elem);
        }
        if (elemName == "loadImage")
        {
            string key = elem->Attribute("key");
            string path = elem->Attribute("path");
            imageManager::addImage(tools::hash("skills"), key, path);
        }
        if (elemName == "images")
        {
            double angle = 0;
            int xAlignment = 0, yAlignment = 0;

            elem->QueryAttribute("angle", &angle);
            elem->QueryAttribute("xAlignment", &xAlignment);
            elem->QueryAttribute("yAlignment", &yAlignment);

            if (elem->Attribute("pathToImages"))
            {
                string pathPattern = elem->Attribute("pathToImages");

                for (unsigned i = 0 ; i < numberOfImages ; ++i)
                {
                    string path = pathPattern;
                    string number = textManager::toString(i, 2);
                    path.replace(path.find_first_of('%'), 2, number);
                    string key = Id + "/" + path;
                    imageManager::addImage(tools::hash("individuals"), key, path, Vector2i(xAlignment, yAlignment), shader);
                    if (hue + saturation + luminance != 0)
                        imageManager::changeHSL(tools::hash("individuals"), key, hue, saturation, luminance);

                    addImage(angle * M_PI / 180.0, i, key);
                }
            }

            if (elem->Attribute("imageFile"))
            {
                string path = elem->Attribute("imageFile");
                string key = Id + "/" + path;
                imageManager::addImage(tools::hash("individuals"), key, path, Vector2i(xAlignment, yAlignment), shader);
                addImage(angle * M_PI / 180.0, 0, key);
            }
        }
        if (elemName == "script")
        {
            scriptString = elem->GetText();
            loadScript();
        }

        elem = elem->NextSiblingElement();
    }
}

void Skill::loadScript()
{
    script = luaL_newstate();
    luaL_openlibs(script);

    luaL_loadbuffer(script, scriptString.c_str(), scriptString.length(), Id.c_str());

    lua_register(script, "addSound", LUA_addSound);
    lua_register(script, "cout", LUA_cout);

    lua_register(script, "createIndividual", [](lua_State* S)
    {
        Carte* w = devilsai::getResource<Carte>(lua_tostring(S, 1));
        Classe_Commune* s = devilsai::getResource<Classe_Commune>(lua_tostring(S, 2));

        if (w != nullptr && s != nullptr)
        {
            Individu* i = new Individu(s, lua_tonumber(S, 3), lua_tonumber(S, 4));
            w->insertItem(i);
            lua_pushlightuserdata(S, (void*)i);
        }
        else lua_pushlightuserdata(S, 0);

        return 1;
    });

    lua_register(script, "fight", LUA_combat);
    lua_register(script, "getElementInteraction", LUA_getTargetedItem);

    lua_register(script, "individual_changeCurrentSkill", [](lua_State* S)
    {
        Individu* ind = static_cast<Individu*>(lua_touserdata(S, 1));
        if (ind != nullptr)
            ind->Set_Activite(lua_tostring(S, 2));
        return 0;
    });

    lua_register(script, "individual_copy", LUA_individual_copy);
    lua_register(script, "individual_get", LUA_get);

    lua_register(script, "individual_set", [](lua_State* S)
    {
        Individu* ind = static_cast<Individu*>(lua_touserdata(S, 1));
        string_view field = lua_tostring(S, 2);

        if (ind == nullptr) return 0;

        if (field == "life")
            ind->setHealthStatus(Life, lua_tonumber(S, 3));
        else if (field == "energy")
            ind->setHealthStatus(Energy, lua_tonumber(S, 3));
        else if (field == "owner")
            ind->setOwner(static_cast<Individu*>(lua_touserdata(S, 3)));

        return 0;
    });

    lua_register(script, "isIndividu", LUA_isIndividu);
    lua_register(script, "playSound", LUA_playSound);
    lua_register(script, "useObject", LUA_useObject);

    lua_pcall(script, 0, 0, 0);

    lua_atpanic(script, [](lua_State* S)
    {
        tools::debug::error(lua_tostring(S, -1), "lua", __FILENAME__, __LINE__);
        return 0;
    });

    //Check if the file is well-formed

    bool fileComplete = true;

    auto check = [&](const char* f)
    {
        lua_getglobal(script, f);
        if (lua_isnil(script, -1))
        {
            tools::debug::error("The skill '" + Id + "' does not define the symbol '" + string(f) + "'", "lua", __FILENAME__, __LINE__);
            fileComplete = false;
        }
        lua_pop(script, 1);
    };

    check("atBegin");
    check("atEnd");

    if (!fileComplete)
    {
        lua_close(script);
        script = nullptr;
        return;
    }
}

void Skill::atBegin(Individu* owner)
{
    if (script == nullptr)
        return;

    lua_getglobal(script, "atBegin");
    lua_pushlightuserdata(script, (void*)owner);
    lua_call(script, 1, 0);
}

void Skill::atEnd(Individu* owner)
{
    if (script == nullptr)
        return;

    lua_getglobal(script, "atEnd");
    lua_pushlightuserdata(script, (void*)owner);
    lua_call(script, 1, 0);
}
