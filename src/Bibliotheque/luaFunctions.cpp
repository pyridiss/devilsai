
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

#include <iostream>
#include <string>
#include <sstream>

#include <lua.hpp>

#include "tools/debug.h"
#include "tools/signals.h"
#include "tools/timeManager.h"
#include "tools/filesystem.h"
#include "tools/math.h"
#include "textManager/richText.h"
#include "textManager/textManager.h"
#include "musicManager/musicManager.h"

#include "devilsai-resources/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"

#include "gamedata.h"
#include "Jeu/options.h"

int LUA_cout(lua_State* L)
{
	cout << "LUA says: " << lua_tostring(L, 1) << endl;
	return 0;
}

int LUA_getTargetedItem(lua_State* L)
{
    Individu* ind = static_cast<Individu*>(lua_touserdata(L, 1));

    lua_pushlightuserdata(L, (void*)ind->targetedItem());

    return 1;
}

int LUA_isIndividu(lua_State* L)
{
	Element_Carte* elem = (Element_Carte*)lua_touserdata(L, 1);

	Individu* ind = dynamic_cast<Individu*>(elem);

	if (ind != NULL) lua_pushboolean(L, true);
	else lua_pushboolean(L, false);

	return 1;
}

int LUA_combat(lua_State* L)
{
    Element_Carte* a = (Element_Carte*)lua_touserdata(L, 1);
    Element_Carte* b = (Element_Carte*)lua_touserdata(L, 2);
    Individu* indA = dynamic_cast<Individu*>(a);
    Individu* indB = dynamic_cast<Individu*>(b);

    indA->fight(indB);

	return 0;
}

int LUA_set(lua_State* L)
{
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
    else if (field == "life") ind->setHealthStatus(Life, value);
    else if (field == "energy") ind->setHealthStatus(Energy, value);
    else if (field == "healing") ind->setHealthStatus(Healing, value);

	return 0;
}

int LUA_get(lua_State* L)
{
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
    else
    {
        Attribute a = stringToAttribute(field);
        if (a != numberOfAttributes)
            result = ind->currentHealthStatus(a);
    }

	lua_pushnumber(L, result);
	return 1;
}

int LUA_individual_copy(lua_State* L)
{
    Individu* i1 = (Individu*)lua_touserdata(L, 1);
    Individu* i2 = (Individu*)lua_touserdata(L, 2);
    string field = lua_tostring(L, 3);

    if (field == "angle") i2->angle = i1->angle;
    if (field == "diplomacy") i2->Diplomatie = i1->Diplomatie;

    return 0;
}

int LUA_useObject(lua_State* L)
{
    Individu* ind = static_cast<Individu*>(lua_touserdata(L, 1));
	string object = lua_tostring(L, 2);

    auto& objects = ind->inventory.objects;

    for (auto& i : objects)
    {
        if (i.stackable() && i.name() == object)
        {
            //TODO: Erase temporary objects instead of refusing the use of the object.
            if (ind->inventory.at(i.requiredSlot()) != nullptr)
                break;

            if (i.quantity() > 1)
            {
                ind->inventory.addObject(i.name(), i.requiredSlot());
                i.setQuantity(i.quantity() - 1);
            }
            else
            {
                i.setSlot(i.requiredSlot());
                objects.push_back(std::move(i));
                ind->inventory.deleteObject(objects.back());
            }
        }
    }

	return 0;
}

int LUA_addTooltip(lua_State* L)
{
    tools::signals::addSignal("add-tooltip", lua_tostring(L, 1));
    return 0;
}

int LUA_getQuantityOf(lua_State* L)
{
    Individu* ind = static_cast<Individu*>(lua_touserdata(L, 1));
	string object = lua_tostring(L, 2);
    int result = ind->inventory.quantityOf(object);

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
    Individu* ind = gamedata::findIndividuUnique(lua_tostring(L, 1));
	//A quest can ask for an element already dead (KillStolas for example)
//	if (ind == NULL) Erreur("Un élément non chargé ou non Individu_Unique a été demandé", "");

    lua_pushlightuserdata(L, (void*)ind);
	return 1;
}

int LUA_pushDialog(lua_State* L)
{
    string container = lua_tostring(L, 1);
    string newDialog = lua_tostring(L, 2);
    textManager::PlainText plain = textManager::getText(container, newDialog);
    textManager::RichText rich;
    rich.setSize(options::option<unsigned>(tools::math::sdbm_hash("dialog-width")), 0);
    rich.setDefaultProperties("liberation", 12, Color(255, 255, 255));
    rich.addFlags(textManager::HAlignJustify);
    rich.create(plain);

    gamedata::listDialogs().push_back(std::move(rich));
	return 0;
}

int LUA_dialogDisplayed(lua_State* L)
{
    lua_pushboolean(L, gamedata::listDialogs().empty());
	return 1;
}

int LUA_interact(lua_State* L)
{
    Individu* individual = (Individu*)lua_touserdata(L, 1);
    Element_Carte* element = (Element_Carte*)lua_touserdata(L, 2);

    bool result = false;

    if (individual != nullptr && element != nullptr)
        result = individual->interact(element);
    else
        tools::debug::error("LUA_interact() has been called with a non-existent element.", "lua", __FILENAME__, __LINE__);

	lua_pushboolean(L, result);
	return 1;
}

int LUA_loadWorld(lua_State* L)
{
    tools::debug::message("LUA_loadWorld() called", "lua", __FILENAME__, __LINE__);

    string world = lua_tostring(L, 1);
    string file = lua_tostring(L, 2);
    string tag = lua_tostring(L, 3);

    Carte* w = gamedata::world(world);
    if (w == nullptr)
    {
        gamedata::addWorld(world);
        w = gamedata::world(world);
    }

    w->loadFromFile(tools::filesystem::dataDirectory() + file, tag);

    return 0;
}

int LUA_deleteList(lua_State* L)
{
	gamedata::currentWorld()->SupprimerListe(lua_tostring(L, 1));
	return 0;
}

int LUA_I(lua_State* L)
{
	lua_pushnumber(L, tools::timeManager::I(1./60.));
	return 1;
}

int LUA_addCheckPoint(lua_State* L)
{
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
    Element_Carte* elem = (Element_Carte*)lua_touserdata(L, 1);
	gamedata::currentWorld()->SupprimerElement(elem);
	return 0;
}

int LUA_setActivity(lua_State* L)
{
    Individu* ind = static_cast<Individu*>(lua_touserdata(L, 1));

	if (ind != NULL) ind->Set_Activite(lua_tostring(L, 2));

	return 0;
}

int LUA_possess(lua_State* L)
{
    Individu* ind = static_cast<Individu*>(lua_touserdata(L, 1));

    string_view object = lua_tostring(L, 2);

	bool result = false;
	for (auto& i : ind->inventory.objects)
	{
        if (i.name() == object)
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
    Individu* indA = static_cast<Individu*>(lua_touserdata(L, 1));
    Individu* indB = static_cast<Individu*>(lua_touserdata(L, 2));

    string_view object = lua_tostring(L, 3);

    auto iObj = indA->inventory.objects.begin();

    WearableItem tmp;

    //Retrieve the item in the tmp variable
    while (iObj != indA->inventory.objects.end() && iObj->name() != object)
        ++iObj;
    if (iObj == indA->inventory.objects.end()) return 0;

    tmp = std::move(*iObj);
    indA->inventory.objects.erase(iObj);

    //Now, we check is the same object exists in the player inventory and can be stacked.
    for (auto& i : indB->inventory.objects)
    {
        if (i.stackable() && i.name() == tmp.name())
        {
            i.setQuantity(i.quantity() + tmp.quantity());
            //We're done, tmp will be destroyed
            return 0;
        }
    }

    //The item cannot be stacked, we try to find an empty slot
    int key = 1;
    while (indB->inventory.at("inventory" + textManager::toString(key, 2)) != nullptr)
        ++key;
    //TODO: this function may push an item outside the inventory range; fix this

    tmp.setSlot("inventory" + textManager::toString(key, 2));
    indB->inventory.objects.push_back(std::move(tmp));

    return 0;
}

int LUA_addSound(lua_State* L)
{
	string sound = lua_tostring(L, 1);
    musicManager::addSound(sound);
	return 0;
}

int LUA_playSound(lua_State* L)
{
	string sound = lua_tostring(L, 1);
    musicManager::playSound(sound);
	return 0;
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

int LUA_moveItemTo(lua_State* L)
{
    tools::debug::message("LUA_moveItemTo() called", "lua", __FILENAME__, __LINE__);

    Individu* ind = (Individu*)lua_touserdata(L, 1);
    string w = lua_tostring(L, 2);
    string p = lua_tostring(L, 3);

    Carte* newWorld = gamedata::world(w);
    if (newWorld == nullptr)
    {
        tools::debug::error("World not found: " + w, "lua", __FILENAME__, __LINE__);
        return 0;
    }

    tools::math::Shape* zone = nullptr;
    for (auto & place : newWorld->places)
    {
        if (place.first->Type == p) zone = &(place.first->size);
    }

    if (zone == nullptr)
    {
        tools::debug::error("This place does not exist: " + p, "lua", __FILENAME__, __LINE__);
        return 0;
    }

    pair<tools::math::Vector2d, tools::math::Vector2d> box = zone->box;
    tools::math::Vector2d originalPosition = ind->position();

    int debugCounter = 0;
    do
    {
        ++debugCounter;

        //Set a new random position
        double x = box.first.x + (box.second.x - box.first.x)/10000.0 * (double)(rand()%10000);
        double y = box.first.y + (box.second.y - box.first.y)/10000.0 * (double)(rand()%10000);
        ind->move(x - ind->position().x, y - ind->position().y);

        //1. The item must collide with the zone
        if (!tools::math::intersection(ind->size, *zone)) continue;

        //2. The item must not collide with anything else
        if (newWorld->findFirstCollidingItem(ind, ind->size, false).first == nullptr)
            break;
    }
    while (debugCounter < 100);

    if (debugCounter == 100)
    {
        //Refuse to do the move: there is no place left
        ind->move(originalPosition.x - ind->position().x, originalPosition.y - ind->position().y);
        lua_pushboolean(L, false);
    }
    else
    {
        for (auto& world : gamedata::worlds())
        {
            world.second->removeItem(ind);
            world.second->stopManagement();
        }

        newWorld->insertItem(ind);
        lua_pushboolean(L, true);
    }

    return 1;
}
