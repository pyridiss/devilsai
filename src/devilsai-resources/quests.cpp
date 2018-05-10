
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

#include <lua.hpp>
#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/signals.h"
#include "tools/filesystem.h"
#include "tools/math.h"
#include "tools/timeManager.h"
#include "textManager/textManager.h"

#include "../Bibliotheque/luaFunctions.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu/options.h"

#include "devilsai-gui/journal.h"
#include "devilsai-gui/console.h"

#include "gamedata.h"

using namespace tinyxml2;

namespace resources::quests {

unordered_map<string, lua_State*> Quests;

void addQuest(string newQuest, string args)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dofile(L, (tools::filesystem::dataDirectory() + newQuest).c_str());

    lua_atpanic(L, [](lua_State* S)
    {
        tools::debug::error(lua_tostring(S, -1), "lua", __FILENAME__, __LINE__);
        return 0;
    });

    //Check if the file is well-formed

    bool fileComplete = true;

    auto check = [&](const char* f)
    {
        lua_getglobal(L, f);
        if (lua_isnil(L, -1))
        {
            tools::debug::error("The quest '" + newQuest + "' does not define the symbol '" + string(f) + "'", "lua", __FILENAME__, __LINE__);
            fileComplete = false;
        }
        lua_pop(L, 1);
    };

    check("questBegin");
    check("questManage");
    check("questIsDone");
    check("questSave");
    check("questRecoverState");
    check("questEnd");

    if (!fileComplete)
    {
        lua_close(L);
        return;
    }

    lua_register(L, "dispRemainingEnemies", [](lua_State* S)
    {
        textManager::PlainText a = textManager::getText("devilsai", "MONSTRES_RESTANTS");
        a.addParameter((unsigned)lua_tonumber(S,1));
        addConsoleEntry(a);
        return 0;
    });

    lua_register(L, "addQuest", [](lua_State* S)
    {
        addQuest(lua_tostring(S, 1), "true");
        return 0;
    });

    lua_register(L, "addExperience", [](lua_State* S)
    {
        gamedata::player()->gainExperience(lua_tonumber(S, 1));
        return 0;
    });

    lua_register(L, "toBlack", [](lua_State* S)
    {
        tools::debug::warning("lua function 'toBlack' is not implemented.", "lua", __FILENAME__, __LINE__);
        return 0;
    });

    lua_register(L, "gameSuccessfullyEnded", [](lua_State* S)
    {
        tools::signals::addSignal("game-successfully-ended");
        tools::signals::addSignal("new-game");
        return 0;
    });

    lua_register(L, "enableCinematics", [](lua_State* S)
    {
        options::addOption<bool>(tools::hash("cinematic-mode"), lua_toboolean(S, 1));
        return 0;
    });

    lua_register(L, "addJournalEntry", [](lua_State* S)
    {
        addJournalEntry(lua_tostring(S, 1), lua_tostring(S, 2), lua_tostring(S, 3));
        return 0;
    });

    lua_register(L, "collision", [](lua_State* S)
    {
        Element_Carte* i1 = static_cast<Element_Carte*>(lua_touserdata(S, 1));
        Element_Carte* i2 = static_cast<Element_Carte*>(lua_touserdata(S, 2));
        lua_pushboolean(S, tools::math::intersection(i1->size, i2->size));
        return 1;
    });

    lua_register(L, "getTimeElapsed", [](lua_State* S)
    {
        long old = lua_tonumber(S, 1);
        lua_pushnumber(S, tools::timeManager::timeElapsed() - old);
        return 1;
    });

    lua_register(L, "questRunning", [](lua_State* S)
    {
        string quest = lua_tostring(S, 1);

        bool result = false;

        if (Quests.find(quest) != Quests.end())
            result = true;

        lua_pushboolean(S, result);
        return 1;
    });

    lua_register(L, "resetTimer", [](lua_State* S)
    {
        lua_pushnumber(S, tools::timeManager::timeElapsed());
        return 1;
    });

	lua_register(L, "cout", LUA_cout);
	lua_register(L, "getNumberOfItemsByTag", LUA_getNumberOfItemsByTag);
	lua_register(L, "getElement", LUA_getElement);
	lua_register(L, "pushDialog", LUA_pushDialog);
	lua_register(L, "dialogDisplayed", LUA_dialogDisplayed);
	lua_register(L, "interact", LUA_interact);
    lua_register(L, "loadWorld", LUA_loadWorld);
	lua_register(L, "deleteList", LUA_deleteList);
	lua_register(L, "I", LUA_I);
	lua_register(L, "addCheckPoint", LUA_addCheckPoint);
	lua_register(L, "deleteElement", LUA_deleteElement);
	lua_register(L, "setActivity", LUA_setActivity);
	lua_register(L, "possess", LUA_possess);
	lua_register(L, "transferObject", LUA_transferObject);
	lua_register(L, "get", LUA_get);
	lua_register(L, "set", LUA_set);

	lua_getglobal(L, "questBegin");
	lua_pushstring(L, args.c_str());
	lua_call(L, 1, 0);

    Quests.emplace(newQuest, L);
}

void manageQuests()
{
	for (auto i = Quests.begin() ; i != Quests.end() ; )
	{
		lua_getglobal(i->second, "questManage");
		lua_call(i->second, 0, 0);

		lua_getglobal(i->second, "questIsDone");
		lua_call(i->second, 0, 1);

		bool done = lua_toboolean(i->second, -1);
        lua_pop(i->second, 1);

		if (done)
		{
			lua_State *j = i->second;
			lua_getglobal(j, "questEnd");
			lua_call(j, 0, 0);

			i = Quests.erase(i);

			lua_close(j);
		}
		else ++i;
	}
}

void clear()
{
    for (auto& q : Quests)
        lua_close(q.second);
    Quests.clear();
}

void save(XMLDocument& doc, tinyxml2::XMLElement* root)
{
    for (auto& i : Quests)
    {
        XMLElement* quest = doc.NewElement("quest");
        quest->SetAttribute("file", i.first.c_str());
        quest->SetAttribute("initialData", "false");

        lua_getglobal(i.second, "questSave");
        lua_call(i.second, 0, 1);
        quest->SetAttribute("currentState", lua_tostring(i.second, -1));

        root->InsertEndChild(quest);
    }
}

void load(XMLElement* elem)
{
    string questFile = elem->Attribute("file");
    string initialData, currentState;
    if (elem->Attribute("initialData"))
    {
        initialData = elem->Attribute("initialData");
    }
    if (elem->Attribute("currentState"))
    {
        currentState = elem->Attribute("currentState");
    }

    addQuest(questFile, initialData);

    if (!currentState.empty())
    {
        auto i = Quests.find(questFile);
        if (i != Quests.end())
        {
            lua_getglobal(i->second, "questRecoverState");
            lua_pushstring(i->second, currentState.c_str());
            lua_call(i->second, 1, 0);
        }
        else tools::debug::error("Error while loading quest " + questFile, "files", __FILENAME__, __LINE__);
    }
}

} // namespace resources::quests
