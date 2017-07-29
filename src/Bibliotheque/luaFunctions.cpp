
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

#include <string>
#include <sstream>

#include <lua5.2/lua.hpp>

#include "tools/debug.h"
#include "tools/timeManager.h"
#include "tools/filesystem.h"
#include "musicManager/musicManager.h"

#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "../Jeu/Jeu.h"
#include "Bibliotheque.h"

#include "gamedata.h"

int LUA_panic(lua_State* L)
{
	Erreur("LUA PANIC: ", lua_tostring(L, -1));

	return 0;
}

int LUA_cout(lua_State* L)
{
	cout << "LUA says: " << lua_tostring(L, 1) << endl;
	return 0;
}

int LUA_getElementById(lua_State* L)
{
    MESSAGE("LUA_getElementById() called", LUA)

    Element_Carte* elem = gamedata::findElement(lua_tonumber(L, 1));

    if (elem != NULL)
        lua_pushlightuserdata(L, (void*)elem);
    else
        lua_pushnumber(L, 0);

	return 1;
}

int LUA_getElementInteraction(lua_State* L)
{
    MESSAGE("LUA_getElementInteraction() called", LUA)

    Joueur* player = (Joueur*)lua_touserdata(L, 1);
    Element_Carte* elem = NULL;

    if (player != NULL && player->ElementInteraction != -1)
        elem = gamedata::findElement(player->ElementInteraction);

    if (elem != NULL)
        lua_pushlightuserdata(L, (void*)elem);
    else
        lua_pushnumber(L, 0);

	return 1;
}

int LUA_isIndividu(lua_State* L)
{
    MESSAGE("LUA_isIndividu() called", LUA)

	Element_Carte* elem = (Element_Carte*)lua_touserdata(L, 1);

	Individu* ind = dynamic_cast<Individu*>(elem);

	if (ind != NULL) lua_pushboolean(L, true);
	else lua_pushboolean(L, false);

	return 1;
}

int LUA_combat(lua_State* L)
{
    MESSAGE("LUA_combat() called", LUA)

    Element_Carte* a = (Element_Carte*)lua_touserdata(L, 1);
    Element_Carte* b = (Element_Carte*)lua_touserdata(L, 2);
    Individu* indA = dynamic_cast<Individu*>(a);
    Individu* indB = dynamic_cast<Individu*>(b);

	Combat(indA, indB, L);

	return 0;
}

int LUA_set(lua_State* L)
{
    MESSAGE("LUA_set() called", LUA)

    Element_Carte* elem = (Element_Carte*)lua_touserdata(L, 1);
	string field = lua_tostring(L, 2);
	double value = lua_tonumber(L, 3);

	Individu*		ind  = dynamic_cast<Individu*>(elem);

	if (field == "vitality")		if (ind != NULL) ind->Set_Vitalite(value);
	if (field == "energy")			if (ind != NULL) ind->Set_Energie(value);
	if (field == "recuperation")	if (ind != NULL) ind->Set_Recuperation(value);
	if (field == "Diplomatie")		if (elem != NULL) elem->Diplomatie = (int)value;
	if (field == "Num_Max")		if (ind != NULL) ind->Get_Activite(ind->Get_Act())->numberOfImages = (int)value;
	if (field == "strength")			if (ind != NULL) (*ind->Get_Caracs())["strength"] = value;
	if (field == "power")		if (ind != NULL) (*ind->Get_Caracs())["power"] = value;
	if (field == "agility")			if (ind != NULL) (*ind->Get_Caracs())["agility"] = value;
	if (field == "intellect")	if (ind != NULL) (*ind->Get_Caracs())["intellect"] = value;

	return 0;
}

int LUA_get(lua_State* L)
{
    MESSAGE("LUA_get() called", LUA)

	float result = 0;

    Element_Carte* elem = (Element_Carte*)lua_touserdata(L, 1);
	string field = lua_tostring(L, 2);

	Individu* ind = dynamic_cast<Individu*>(elem);
	Individu_Unique* ind_unique = dynamic_cast<Individu_Unique*>(elem);

	//First, we get statistics and characteristics
	if (ind_unique != NULL)	result = ind_unique->get(field);
	else if (ind != NULL)	result = ind->get(field);

	if (result == Jeu.floatNotFound)
	{
		if (field == "PosX")	if (ind != NULL) result = ind->position().x;
		if (field == "PosY")	if (ind != NULL) result = ind->position().y;
        if (field == "angle")   if (ind != NULL) result = ind->angle;
        if (field == "diplomacy") if (ind != NULL) result = ind->Diplomatie;
	}

	lua_pushnumber(L, result);
	return 1;
}

int LUA_individual_copy(lua_State* L)
{
    MESSAGE("LUA_individual_copy() called", LUA)

    Individu* i1 = (Individu*)lua_touserdata(L, 1);
    Individu* i2 = (Individu*)lua_touserdata(L, 2);
    string field = lua_tostring(L, 3);

    if (field == "angle") i2->angle = i1->angle;
    if (field == "diplomacy") i2->Diplomatie = i1->Diplomatie;

    return 0;
}

int LUA_useObject(lua_State* L)
{
    MESSAGE("LUA_useObject() called", LUA)

    Individu_Unique* ind = (Individu_Unique*)lua_touserdata(L, 1);
	string object = lua_tostring(L, 2);

	mapObjects* objects = &(ind->Get_Caracs()->objects.objects);

	mapObjects::iterator i = objects->begin();
	string key = intToString(CLEF_INVENTAIRE);
	while (i != objects->end() && (getStringFromLUA(i->second, "getFileName") != object || i->first[0] != key[0])) ++i;

	if (i != objects->end())
	{
		mapObjects::iterator j = objects->find(getStringFromLUA(i->second, "getIdEmplacement"));
		//On vérifie que l'emplacement correspondant dans l'Équipement est vide
		if (j == objects->end())
		{
			ind->Get_Caracs()->objects.addObject(getStringFromLUA(i->second, "getFileName"), getStringFromLUA(i->second, "getIdEmplacement"));

			j = objects->find(getStringFromLUA(i->second, "getIdEmplacement"));
			setIntToLUA(j->second, "setQuantite", 1);

 			mapObjects::iterator i = objects->begin();
			while (i != objects->end() && getStringFromLUA(i->second, "getFileName") != object) ++i;

			if (getIntFromLUA(i->second, "getQuantite") > 1)
				setIntToLUA(i->second, "setQuantite", getIntFromLUA(i->second, "getQuantite") - 1);
			else
			{
				ind->Get_Caracs()->objects.deleteObject(i->second);
				objects->erase(i);
			}
		}
	}
	return 0;
}

int LUA_dispText(lua_State* L)
{
    MESSAGE("LUA_dispText() called", LUA)

	Disp_Texte(lua_tostring(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), Color(255,255,255), 12.);
	return 0;
}

int LUA_getQuantityOf(lua_State* L)
{
    MESSAGE("LUA_getQuantityOf() called", LUA)

    Individu_Unique* ind = (Individu_Unique*)lua_touserdata(L, 1);
	string object = lua_tostring(L, 2);
	int result = 0;

	mapObjects* objects = &(ind->Get_Caracs()->objects.objects);

	mapObjects::iterator i = objects->begin();
	string key = intToString(CLEF_INVENTAIRE);
	while (i != objects->end() && (getStringFromLUA(i->second, "getFileName") != object || i->first[0] != key[0])) ++i;

	if (i != objects->end())
		result = getIntFromLUA(i->second, "getQuantite");

	lua_pushnumber(L, result);
	return 1;
}

int LUA_getNumberEnemiesInList(lua_State* L)
{
    MESSAGE("LUA_getNumberEnemiesInList() called", LUA)

	string list = lua_tostring(L, 1);
	int qty = 0;
	for (auto& tmp : gamedata::currentWorld()->elements)
		if (tmp->Liste == list && tmp->Type != "TYPE_CADAVRE") ++qty;

	lua_pushnumber(L, qty);
	return 1;
}

int LUA_getElement(lua_State* L)
{
    MESSAGE("LUA_getElement() called", LUA)

	Individu_Unique* ind = gamedata::findIndividuUnique(lua_tostring(L, 1));
	//A quest can ask for an element already dead (KillStolas for example)
//	if (ind == NULL) Erreur("Un élément non chargé ou non Individu_Unique a été demandé", "");

    lua_pushlightuserdata(L, (void*)ind);
	return 1;
}

int LUA_pushDialog(lua_State* L)
{
    MESSAGE("LUA_pushDialog() called", LUA)

	string newDialog = lua_tostring(L, 1);
	Dialog dialog;
	Partie.listDialogs.push_back(dialog);
	Partie.listDialogs.back().load(newDialog + ".lng");
	return 0;
}

int LUA_popDialog(lua_State* L)
{
    MESSAGE("LUA_popDialog() called", LUA)

	string dialog = lua_tostring(L, 1);
	for (auto i = Partie.listDialogs.begin() ; i != Partie.listDialogs.end() ; ++i)
		if (i->id == dialog + ".lng")
		{
			Partie.listDialogs.erase(i);
			break;
		}
	return 0;
}

int LUA_dialogDisplayed(lua_State* L)
{
    MESSAGE("LUA_dialogDisplayed() called", LUA)

	string dialog = lua_tostring(L, 1);
	for (auto& i : Partie.listDialogs)
		if (i.id == dialog + ".lng")
		{
			lua_pushboolean(L, false);
			return 1;
		}

	lua_pushboolean(L, true);
	return 1;
}

int LUA_interact(lua_State* L)
{
    MESSAGE("LUA_interact() called", LUA)

    Individu* individual = (Individu*)lua_touserdata(L, 1);
    Element_Carte* element = (Element_Carte*)lua_touserdata(L, 2);
    bool result = intersection(individual->interactionField, element->size);//false; //= Collision_cercle_cercle(indA->position().x, indA->position().y, indA->size.r1+2, indB->position().x, indB->position().y, indB->size.r1+2);

	lua_pushboolean(L, result);
	return 1;
}

int LUA_loadList(lua_State* L)
{
    MESSAGE("LUA_loadList() called", LUA)

	string idList = lua_tostring(L, 1);
	Load_Carte(idList, TYPE_LISTE);
	return 0;
}

int LUA_loadWorld(lua_State* L)
{
    tools::debug::message("LUA_loadWorld() called", "lua");

    string world = lua_tostring(L, 1);
    string file = lua_tostring(L, 2);
    string tag = lua_tostring(L, 3);

    Carte* w = gamedata::world(world);
    if (w != nullptr)
    {
        gamedata::addWorld(world);
        w = gamedata::world(world);
    }

    w->loadFromFile(tools::filesystem::dataDirectory() + file, tag);

    return 0;
}

int LUA_deleteList(lua_State* L)
{
    MESSAGE("LUA_deleteList() called", LUA)

	gamedata::currentWorld()->SupprimerListe(lua_tostring(L, 1));
	return 0;
}

int LUA_I(lua_State* L)
{
    MESSAGE("LUA_I() called", LUA)

	lua_pushnumber(L, tools::timeManager::I(1./60.));
	return 1;
}

int LUA_addCheckPoint(lua_State* L)
{
    MESSAGE("LUA_addCheckPoint() called", LUA)

    string world = lua_tostring(L, 1);
    int x = lua_tonumber(L, 2);
    int y = lua_tonumber(L, 3);
    int w = lua_tonumber(L, 4);
    int h = lua_tonumber(L, 5);

    CheckPoint *c = gamedata::world(world)->addCheckPoint("CheckPoints", x, y);
    c->size.rectangle(tools::math::Vector2d(-w, -h), tools::math::Vector2d(w, -h), tools::math::Vector2d(-w, h));
    lua_pushlightuserdata(L, (void*)c);

    return 1;
}

int LUA_deleteElement(lua_State* L)
{
    MESSAGE("LUA_deleteElement() called", LUA)

    Element_Carte* elem = (Element_Carte*)lua_touserdata(L, 1);
	gamedata::currentWorld()->SupprimerElement(elem);
	return 0;
}

int LUA_setActivity(lua_State* L)
{
    MESSAGE("LUA_setActivity() called", LUA)

    Individu_Unique* ind = (Individu_Unique*)lua_touserdata(L, 1);

	if (ind != NULL) ind->Set_Activite(lua_tostring(L, 2));

	return 0;
}

int LUA_possess(lua_State* L)
{
    MESSAGE("LUA_possess() called", LUA)

    Individu_Unique* ind = (Individu_Unique*)lua_touserdata(L, 1);

	int object = lua_tonumber(L, 2);

	bool result = false;
	for (auto& i : ind->Get_Caracs()->objects.objects)
	{
		if (getIntFromLUA(i.second, "getInternalNumber") == object)
		{
			result = true;
			break;
		}
	}

	lua_pushboolean(L, result);
	return 1;
}

int LUA_transferObject(lua_State* L)
{
    MESSAGE("LUA_transferObject() called", LUA)

    Individu_Unique* indA = (Individu_Unique*)lua_touserdata(L, 1);
    Individu_Unique* indB = (Individu_Unique*)lua_touserdata(L, 2);

	int object = lua_tonumber(L, 3);

	mapObjects::iterator iObj = indA->Get_Caracs()->objects.objects.begin();

	//1. On place l'objet en question dans Partie.ObjetSelectionne
	while (iObj != indA->Get_Caracs()->objects.objects.end() && getIntFromLUA(iObj->second, "getInternalNumber") != object)
		++iObj;
	if (iObj == indA->Get_Caracs()->objects.objects.end()) return 0;

	Partie.selectedObject = iObj->second;
	setStringToLUA(iObj->second, "setKey", "0");
	indA->Get_Caracs()->objects.objects.erase(iObj);

	//2. On place l'objet dans l'inventaire du receveur
	iObj = indB->Get_Caracs()->objects.objects.begin();

	bool cumulate = false;

	if (getBoolFromLUA(Partie.selectedObject, "getCumul"))
	{
		string key = intToString(CLEF_INVENTAIRE);
		for (mapObjects::iterator k = indB->Get_Caracs()->objects.objects.begin() ; k != indB->Get_Caracs()->objects.objects.end() ; ++k)
			if (k->first[0] == key[0] && 
				getIntFromLUA(k->second, "getInternalNumber") == getIntFromLUA(Partie.selectedObject, "getInternalNumber"))
			{
				cumulate = true;
				iObj = k;
				break;
			}
	}

	if (cumulate)
	{
		setIntToLUA(iObj->second, "setQuantite", getIntFromLUA(iObj->second, "getQuantite") + getIntFromLUA(Partie.selectedObject, "getQuantite"));
		Partie.selectedObject = nullptr;
	}
	else
	{
		int cell = 0;
		string key = intToString(CLEF_INVENTAIRE);
		while (indB->Get_Caracs()->objects.objects.find(key) != indB->Get_Caracs()->objects.objects.end())
		{
			++cell;
			key = intToString(CLEF_INVENTAIRE + cell);
		}
		pair<mapObjects::iterator, bool> result = indB->Get_Caracs()->objects.objects.insert(mapObjects::value_type(key, Partie.selectedObject));
		if (result.second)
		{
			setStringToLUA(result.first->second, "setKey",key);
			Partie.selectedObject = nullptr;
		}
	}

	return 0;
}

int LUA_questRunning(lua_State* L)
{
    MESSAGE("LUA_questRunning() called", LUA)

	string quest = lua_tostring(L, 1);

	bool result = false;

	if (Partie.quests.find(quest) != Partie.quests.end())
		result = true;

	lua_pushboolean(L, result);
	return 1;
}

int LUA_loadElement(lua_State* L)
{
    MESSAGE("LUA_loadElement() called", LUA)

	//Be careful: only the last element is included in collision list
	stringstream stream;
	stream << lua_tostring(L, 1);
	string list = lua_tostring(L, 2);

	Element_Carte* elem = loadElementsFromStream(stream, gamedata::currentWorld(), list);

    lua_pushlightuserdata(L, (void*)elem);
	return 1;
}

int LUA_createActivite(lua_State* L)
{
    MESSAGE("LUA_createActivite() called", LUA)

    Individu_Unique* ind = (Individu_Unique*)lua_touserdata(L, 1);
	string id = lua_tostring(L, 2);

	ind->Ajouter_Activite(id);
	Activite* act = ind->Get_Activite(id);

    lua_pushlightuserdata(L, (void*)act);
	return 1;
}

int LUA_activiteSet(lua_State* L)
{
    MESSAGE("LUA_activiteSet() called", LUA)

    Activite* act = (Activite*)lua_touserdata(L, 1);
	string field = lua_tostring(L, 2);
	int value = lua_tonumber(L, 3);

	if (field == "speed")		if (act != NULL)	act->speed = value;
	if (field == "step")		if (act != NULL)	act->step = value;
	if (field == "priority")	if (act != NULL)	act->priority = value;

	return 0;
}

int LUA_activiteAddImage(lua_State* L)
{
    MESSAGE("LUA_activiteAddImage() called", LUA)
    Erreur("LUA_activiteAddImage() not implemented.", "");
/*
	Activite* act = (Activite*)lua_touserdata(L, 1);
	int Act = lua_tonumber(L, 2);
	int Num = lua_tonumber(L, 3);

	if (act != NULL) act->addImage(Act, Num);

	return 0;
    */
}

int LUA_addSound(lua_State* L)
{
    MESSAGE("LUA_addSound() called", LUA)

	string sound = lua_tostring(L, 1);
    musicManager::addSound(sound);
	return 0;
}

int LUA_playSound(lua_State* L)
{
    MESSAGE("LUA_playSound() called", LUA)

	string sound = lua_tostring(L, 1);
    musicManager::playSound(sound);
	return 0;
}

int LUA_createIndividual(lua_State* L)
{
    MESSAGE("LUA_createIndividual() called", LUA)

    string type = lua_tostring(L, 1);
    double x = lua_tonumber(L, 2);
    double y = lua_tonumber(L, 3);

    Individu* i = gamedata::currentWorld()->AjouterElement_Commun(type, "default", x, y);

    lua_pushlightuserdata(L, (void*)i);

    return 1;
}
