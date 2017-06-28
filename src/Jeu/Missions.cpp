
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

#include <fstream>

#include <lua5.2/lua.hpp>

#include "tools/filesystem.h"

#include "../Bibliotheque/luaFunctions.h"
#include "../Bibliotheque/Templates.h"
#include "../ElementsCarte/ElementsCarte.h"


void addQuest(string newQuest, string args)
{
	MESSAGE("Ajout de la quête " + newQuest + " en cours…", FICHIER)

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dofile(L, (tools::filesystem::dataDirectory() + "quest/" + newQuest + ".lua").c_str());

	lua_atpanic(L, LUA_panic);

	lua_register(L, "dispRemainingEnemies", [](lua_State* L) {
		Ajouter_LigneAmelioration(getFormatedTranslatedMessage(_MONSTRES_RESTANTS, (unsigned)lua_tonumber(L,1)), Color(255, 255, 255, 255));
		return 0;
	});

	lua_register(L, "addQuest",			[](lua_State* L) { addQuest(lua_tostring(L, 1), "true"); return 0; });
	lua_register(L, "addExperience",	[](lua_State* L) { Partie.perso->GainExperience(NULL, 0, lua_tonumber(L, 1)); return 0; });
	lua_register(L, "toBlack",			[](lua_State* L) { Disp_FonduNoir(lua_toboolean(L, 1) ? 1 : -1); return 0; });
	lua_register(L, "loadFirstChapter",	[](lua_State* L) { Partie.loadFirstChapter = true; return 0; });
	lua_register(L, "enableCinematics",	[](lua_State* L) { Partie.ModeCinematiques = lua_toboolean(L, 1); return 0; });
	lua_register(L, "addJournalEntry",	[](lua_State* L) { Partie.journal.addEntry(lua_tostring(L, 1)); return 0; });
	lua_register(L, "journalEntryDone",	[](lua_State* L) { Partie.journal.setDone(lua_tostring(L, 1)); return 0; });

	lua_register(L, "cout", LUA_cout);
	lua_register(L, "getNumberEnemiesInList", LUA_getNumberEnemiesInList);
	lua_register(L, "getElement", LUA_getElement);
	lua_register(L, "pushDialog", LUA_pushDialog);
	lua_register(L, "popDialog", LUA_popDialog);
	lua_register(L, "dialogDisplayed", LUA_dialogDisplayed);
	lua_register(L, "interact", LUA_interact);
	lua_register(L, "loadList", LUA_loadList);
	lua_register(L, "deleteList", LUA_deleteList);
	lua_register(L, "I", LUA_I);
	lua_register(L, "addActionneur", LUA_addActionneur);
	lua_register(L, "deleteElement", LUA_deleteElement);
	lua_register(L, "setActivity", LUA_setActivity);
	lua_register(L, "possess", LUA_possess);
	lua_register(L, "transferObject", LUA_transferObject);
	lua_register(L, "questRunning", LUA_questRunning);
	lua_register(L, "loadElement", LUA_loadElement);
	lua_register(L, "get", LUA_get);
	lua_register(L, "set", LUA_set);

	lua_getglobal(L, "questBegin");
	lua_pushstring(L, args.c_str());
	lua_call(L, 1, 0);

	Partie.quests.insert(map<string, lua_State*>::value_type(newQuest, L));

	MESSAGE("Quête " + newQuest + " ajoutée.", FICHIER)
}

void manageQuests()
{
	for (map<string, lua_State*>::iterator i = Partie.quests.begin() ; i != Partie.quests.end() ; )
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

			i = Partie.quests.erase(i);

			lua_close(j);
		}
		else ++i;
	}

	if (!Partie.listDialogs.empty())
	{
		bool done = Partie.listDialogs.begin()->display();
		if (done) Partie.listDialogs.pop_front();
	}
}

void saveQuests(ofstream& stream)
{
	for (map<string, lua_State*>::iterator i = Partie.quests.begin() ; i != Partie.quests.end() ; ++i)
	{
		lua_getglobal(i->second, "questSave");
		lua_call(i->second, 0, 1);
		string data = lua_tostring(i->second, -1);
		stream << "QUEST " << i->first << " " << data << endl;
	}
}

void loadQuestFromSavedGame(string quest, string data)
{
	addQuest(quest, "false");
	map<string, lua_State*>::iterator i = Partie.quests.find(quest);
	lua_getglobal(i->second, "questRecoverState");
	lua_pushstring(i->second, data.c_str());
	lua_call(i->second, 1, 0);
}

void deleteQuests()
{
	for (map<string, lua_State*>::iterator i = Partie.quests.begin() ; i != Partie.quests.end() ; )
	{
		lua_State *j = i->second;
		i = Partie.quests.erase(i);
		lua_close(j);
	}
}

/** not yet implemented with LUA :
int Mission::Gestion()
{
	int compteur = 0;
	int Retour = 0;

	Element_Carte *tmp = NULL;
	Individu_Unique* ind = Partie.perso;
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

		case OBJECTIF_SUIVRE_PERSO :	if (DonneesString[DONNEE_CARTE] != Partie.CarteCourante->Id) break;
										tmp = Partie.CarteCourante->head;
										while (tmp != NULL)
										{
											if (tmp->Liste == DonneesString[DONNEE_LISTE])
											{
												tmp->PosX = Partie.perso->PosX;
												tmp->PosY = Partie.perso->PosY;
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