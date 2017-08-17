
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

    Individu* ind  = dynamic_cast<Individu*>(elem);

    if (ind == NULL)
    {
        tools::debug::error("LUA_set() has been called with a non-individual pointer.", "lua", __FILENAME__, __LINE__);
        return 0;
    }

    if (field == "Diplomatie") elem->Diplomatie = (int)value;
    else if (field == "Num_Max") ind->Get_Activite(ind->Get_Act())->numberOfImages = (int)value;
    else if (field == "vitality") ind->setHealthStatus(Statistiques::Life, value);
    else if (field == "energy") ind->setHealthStatus(Statistiques::Energy, value);
    else if (field == "healing") ind->setHealthStatus(Statistiques::Healing, value);

	return 0;
}

int LUA_get(lua_State* L)
{
    MESSAGE("LUA_get() called", LUA)

    double result = 0;

    Element_Carte* elem = (Element_Carte*)lua_touserdata(L, 1);
	string field = lua_tostring(L, 2);

	Individu* ind = dynamic_cast<Individu*>(elem);

    if (ind == NULL)
    {
        tools::debug::error("LUA_get() has been called with a non-individual pointer.", "lua", __FILENAME__, __LINE__);
        lua_pushnumber(L, 1);
        return 1;
    }

    if (field == "PosX") result = ind->position().x;
    else if (field == "PosY") result = ind->position().y;
    else if (field == "angle") result = ind->angle;
    else if (field == "diplomacy") result = ind->Diplomatie;
    else if (field == "vitality") result = ind->currentHealthStatus(Statistiques::Life);
    else if (field == "energy") result = ind->currentHealthStatus(Statistiques::Energy);
    else if (field == "healing") result = ind->currentHealthStatus(Statistiques::Healing);
    else if (field == "strength") result = ind->currentHealthStatus(Caracteristiques::Strength);
    else if (field == "power") result = ind->currentHealthStatus(Caracteristiques::Power);
    else if (field == "agility") result = ind->currentHealthStatus(Caracteristiques::Agility);
    else if (field == "intellect") result = ind->currentHealthStatus(Caracteristiques::Intellect);
    else if (field == "constitution") result = ind->currentHealthStatus(Caracteristiques::Constitution);
    else if (field == "charisma") result = ind->currentHealthStatus(Caracteristiques::Charisma);
    else if (field == "dodge") result = ind->currentHealthStatus(Caracteristiques::Dodge);
    else if (field == "healingPower") result = ind->currentHealthStatus(Caracteristiques::HealingPower);
    else if (field == "runSpeed") result = ind->currentHealthStatus(Caracteristiques::RunSpeed);
    else if (field == "attackSpeed") result = ind->currentHealthStatus(Caracteristiques::AttackSpeed);
    else if (field == "injurySpeed") result = ind->currentHealthStatus(Caracteristiques::InjurySpeed);

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

	mapObjects* objects = &(ind->inventory.objects);

    for (auto& i : *objects)
    {
        if (getBoolFromLUA(i.second, "getCumul") && getStringFromLUA(i.second, "getFileName") == object)
        {
            //TODO: Erase temporary objects instead of refusing the use of the object.
            if (objects->find(getStringFromLUA(i.second, "getIdEmplacement")) != objects->end())
                break;

            if (getIntFromLUA(i.second, "getQuantite") > 1)
            {
                ind->inventory.addObject(getStringFromLUA(i.second, "getFileName"), getStringFromLUA(i.second, "getIdEmplacement"));
                setIntToLUA(i.second, "setQuantite", getIntFromLUA(i.second, "getQuantite") - 1);
            }
            else
            {
                setStringToLUA(i.second, "setKey", getStringFromLUA(i.second, "getIdEmplacement"));
                objects->emplace(getStringFromLUA(i.second, "getIdEmplacement"), i.second);
                objects->erase(i.first);
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

	mapObjects* objects = &(ind->inventory.objects);

	mapObjects::iterator i = objects->begin();
	string key = intToString(CLEF_INVENTAIRE);
	while (i != objects->end() && (getStringFromLUA(i->second, "getFileName") != object || i->first[0] != key[0])) ++i;

	if (i != objects->end())
		result = getIntFromLUA(i->second, "getQuantite");

	lua_pushnumber(L, result);
	return 1;
}

int LUA_getNumberOfItemsByTag(lua_State* L)
{
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
    {
        tools::debug::error("LUA_getNumberOfItemsByTag(): bad arguments.", "lua", __FILENAME__, __LINE__);
        lua_pushnumber(L, 0);
        return 1;
    }

    string world = lua_tostring(L, 1);
    string tag = lua_tostring(L, 2);

    auto items = gamedata::getItemsByTag(world, tag);

    lua_pushnumber(L, items.size());
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

    bool result = false;

    if (individual != nullptr && element != nullptr)
        result = intersection(individual->interactionField, element->size);
    else
        tools::debug::error("LUA_interact() has been called with a non-existent element.", "lua", __FILENAME__, __LINE__);

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
    tools::debug::message("LUA_loadWorld() called", "lua", __FILENAME__, __LINE__);

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

    Carte* wo = gamedata::world(world);
    if (wo == nullptr)
    {
        tools::debug::error("LUA_addCheckPoint() has been called from a Lua state that require an unknown world: " + world, "lua", __FILENAME__, __LINE__);
        lua_pushnumber(L, 0);
        return 1;
    }

    CheckPoint *c = wo->addCheckPoint("CheckPoints", x, y);
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
	for (auto& i : ind->inventory.objects)
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

	mapObjects::iterator iObj = indA->inventory.objects.begin();

	//1. On place l'objet en question dans Partie.ObjetSelectionne
	while (iObj != indA->inventory.objects.end() && getIntFromLUA(iObj->second, "getInternalNumber") != object)
		++iObj;
	if (iObj == indA->inventory.objects.end()) return 0;

	Partie.selectedObject = iObj->second;
	setStringToLUA(iObj->second, "setKey", "0");
	indA->inventory.objects.erase(iObj);

	//2. On place l'objet dans l'inventaire du receveur
	iObj = indB->inventory.objects.begin();

	bool cumulate = false;

	if (getBoolFromLUA(Partie.selectedObject, "getCumul"))
	{
		string key = intToString(CLEF_INVENTAIRE);
		for (mapObjects::iterator k = indB->inventory.objects.begin() ; k != indB->inventory.objects.end() ; ++k)
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
		while (indB->inventory.objects.find(key) != indB->inventory.objects.end())
		{
			++cell;
			key = intToString(CLEF_INVENTAIRE + cell);
		}
		pair<mapObjects::iterator, bool> result = indB->inventory.objects.insert(mapObjects::value_type(key, Partie.selectedObject));
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

	if (gamedata::quests().find(quest) != gamedata::quests().end())
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

int LUA_changeCurrentSkill(lua_State* L)
{
    tools::debug::message("LUA_changeCurrentSkill() called", "lua", __FILENAME__, __LINE__);

    Individu* ind = (Individu*)lua_touserdata(L, 1);
    string newSkill = lua_tostring(L, 2);

    ind->Set_Activite(newSkill);
    ind->ActEffectue = false;

    return 0;
}
