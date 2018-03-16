
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

#include "tools/debug.h"
#include "tools/signals.h"
#include "tools/filesystem.h"
#include "textManager/textManager.h"

#include "Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/luaFunctions.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu/options.h"

#include "devilsai-gui/journal.h"
#include "devilsai-gui/console.h"

#include "gamedata.h"


void addQuest(string newQuest, string args)
{
    if (!tools::filesystem::checkFile(tools::filesystem::dataDirectory() + newQuest))
    {
        tools::debug::error("Unable to load quest: checkFile failed (" + newQuest + ").", "files", __FILENAME__, __LINE__);
        return;
    }

	MESSAGE("Ajout de la quête " + newQuest + " en cours…", FICHIER)

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dofile(L, (tools::filesystem::dataDirectory() + newQuest).c_str());

    lua_atpanic(L, [](lua_State* S)
    {
        tools::debug::error(lua_tostring(S, -1), "lua", __FILENAME__, __LINE__);
        return 0;
    });

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
        gamedata::player()->GainExperience(NULL, 0, lua_tonumber(S, 1));
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
        options::addOption<bool>(tools::math::sdbm_hash("cinematic-mode"), lua_toboolean(S, 1));
        return 0;
    });

    lua_register(L, "addJournalEntry", [](lua_State* S)
    {
        addJournalEntry(lua_tostring(S, 1), lua_tostring(S, 2), lua_tostring(S, 3));
        return 0;
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
	lua_register(L, "questRunning", LUA_questRunning);
	lua_register(L, "get", LUA_get);
	lua_register(L, "set", LUA_set);
    lua_register(L, "resetTimer", LUA_resetTimer);
    lua_register(L, "getTimeElapsed", LUA_getTimeElapsed);

	lua_getglobal(L, "questBegin");
	lua_pushstring(L, args.c_str());
	lua_call(L, 1, 0);

	gamedata::quests().insert(map<string, lua_State*>::value_type(newQuest, L));

	MESSAGE("Quête " + newQuest + " ajoutée.", FICHIER)
}

void manageQuests()
{
	for (auto i = gamedata::quests().begin() ; i != gamedata::quests().end() ; )
	{
		lua_getglobal(i->second, "questManage");
		lua_call(i->second, 0, 0);

		lua_getglobal(i->second, "questIsDone");
		lua_call(i->second, 0, 1);

		bool done = lua_toboolean(i->second, -1);

		if (done)
		{
			lua_State *j = i->second;
			lua_getglobal(j, "questEnd");
			lua_call(j, 0, 0);

			i = gamedata::quests().erase(i);

			lua_close(j);
		}
		else ++i;
	}
}

/** not yet implemented with LUA :
int Mission::Gestion()
{
	int compteur = 0;
	int Retour = 0;

	Element_Carte *tmp = NULL;
	Individu_Unique* ind = gamedata::player();
	Individu_Unique* Donneur;
	Individu_Unique* Receveur;
	MapInventaire::iterator equ;
	bool Cumuler = false;
	int Resultat = COLL_OK;

	bool FinMission = true;

	switch (Objectif)
	{

		case OBJECTIF_COLLISION :	if (DonneesString.find(DONNEE_IND1) != DonneesString.end())
									{
										ind = Get_IndividuUnique(DonneesString[DONNEE_IND1]);
										if (ind == NULL) break;
									}
									RaZ_Coll();
									while(Resultat != COLL_END)
									{
										Resultat = ParcoursCollisions(ind);
										if (Resultat == COLL_INTER)
										{
											tmp = Get_Current_Coll();
											if (DonneesString.find(DONNEE_LISTE) != DonneesString.end())
											{
												if (tmp->Liste == DonneesString[DONNEE_LISTE])
												{
													MissionTerminee = true;
													Objectif = 0;
													Resultat = COLL_END;
												}
											}
											else if (DonneesString.find(DONNEE_IND2) != DonneesString.end())
											{
												if (tmp->Type == DonneesString[DONNEE_IND2] ||
													(tmp->Type == "TYPE_CADAVRE" && dynamic_cast<Cadavre*>(tmp)->Ind_Id == DonneesString[DONNEE_IND2]))
												{
													MissionTerminee = true;
													Objectif = 0;
													Resultat = COLL_END;
												}
											}
										}
									}
									break;

		case OBJECTIF_SUIVRE_PERSO :	if (DonneesString[DONNEE_CARTE] != gamedata::currentWorld()->Id) break;
										tmp = gamedata::currentWorld()->head;
										while (tmp != NULL)
										{
											if (tmp->Liste == DonneesString[DONNEE_LISTE])
											{
												tmp->PosX = gamedata::player()->PosX;
												tmp->PosY = gamedata::player()->PosY;
											}
											tmp = tmp->next;
										}
										break;

		default: FinMission = false;
	}

	if (MissionTerminee && FinMission)
	{
		for (ListeFinsMissions::iterator fm = Fins.begin() ; fm != Fins.end() ; ++fm)
		{
			switch (fm->Type)
			{
				case FIN_SUPPRIMER_MISSION :	for (ListeMissions::iterator i = Partie.Missions.begin() ; i != Partie.Missions.end() ; ++i)
												{
													if (i->Numero == fm->DonneeStr)
													{
														Partie.Missions.erase(i);
														i = Partie.Missions.begin();
													}
												}	 
												break;
			}
		}
	}
}
**/
