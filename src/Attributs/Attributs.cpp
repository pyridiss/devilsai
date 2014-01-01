
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

#include <lua5.2/lua.hpp>

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/luaFunctions.h"
#include "../Jeu/Jeu.h"
#include "Attributs.h"


/* Manager for objects */

Objects::~Objects()
{
	deleteObjects();
}

pair<mapObjects::iterator, bool> Objects::addObject(string newObject, string key, int qualityRandomObject)
{
	MESSAGE("Chargement du l'objet " + newObject + "…", FICHIER)

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dofile(L, (Partie.DATA + "object/" + newObject + ".lua").c_str());

	lua_atpanic(L, LUA_panic);

	lua_register(L, "cout", LUA_cout);

	if (qualityRandomObject > 0) setIntToLUA(L, "generateRandomObject", qualityRandomObject);

	//internalNumber should be replaced with newObject
	AjouterImagesObjet(getIntFromLUA(L, "getInternalNumber"));

	pair<mapObjects::iterator, bool> result = objects.insert(mapObjects::value_type(key, L));

	if (result.second) setStringToLUA(L, "setKey", key);
	else
	{
		Erreur("Impossible de créer l'objet demandé :", newObject + " / " + key);
		MESSAGE("… FAIL", FICHIER)
		lua_close(L);
	}

	MESSAGE("… OK", FICHIER)
	return result;
}

void Objects::deleteObject(lua_State* obj)
{
	lua_close(obj);
	obj = NULL;
}

void Objects::saveObjects(ostream& stream)
{
	stream << objects.size() << endl;
	for (mapObjects::iterator i = objects.begin() ; i != objects.end() ; ++i)
	{
		lua_getglobal(i->second, "objectSave");
		lua_call(i->second, 0, 1);
		string data = lua_tostring(i->second, -1);
		stream << getStringFromLUA(i->second, "getFileName") << " " << i->first << " " << data << endl;
	}
}

void Objects::loadObjectFromSavedGame(string object, string key, string data)
{
	const auto& result = addObject(object, key);
	lua_getglobal(result.first->second, "objectRecoverState");
	lua_pushstring(result.first->second, data.c_str());
	lua_call(result.first->second, 1, 0);
}

void Objects::deleteObjects()
{
	for (mapObjects::iterator i = objects.begin() ; i != objects.end() ; )
	{
		lua_State *j = i->second;
		i = objects.erase(i);
		lua_close(j);
	}
}


/* Manager for skills */

void Caracteristiques::addSkill(string newSkill, int owner)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dofile(L, (Partie.DATA + "skill/" + newSkill + ".lua").c_str());

	lua_atpanic(L, LUA_panic);

	lua_register(L, "addImageProjectile", [](lua_State* L) {
		AjouterImageProjectile(lua_tostring(L, 1), lua_tonumber(L, 2)); return 0;
	});

	lua_register(L, "dispImageSkill", [](lua_State* L) {
		Disp_ImageCompetence(lua_tostring(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3)); return 0;
	});

	lua_register(L, "getElementById", LUA_getElementById);
	lua_register(L, "getElementInteraction", LUA_getElementInteraction);
	lua_register(L, "isIndividu", LUA_isIndividu);
	lua_register(L, "collisionCC", LUA_collisionCCwithRayon);
	lua_register(L, "testAngle", LUA_testAngle);
	lua_register(L, "combat", LUA_combat);
	lua_register(L, "createProjectile", LUA_createProjectile);
	lua_register(L, "set", LUA_set);
	lua_register(L, "get", LUA_get);
	lua_register(L, "dirToCoeff", LUA_dirToCoeff);
	lua_register(L, "useObject", LUA_useObject);
	lua_register(L, "dispText", LUA_dispText);
	lua_register(L, "getQuantityOf", LUA_getQuantityOf);
	lua_register(L, "I", LUA_I);
	lua_register(L, "cout", LUA_cout);

	lua_getglobal(L, "skillBegin");
	lua_pushnumber(L, owner);
	lua_call(L, 1, 0);

	//Should be replaced with newSkill
	AjouterImageCompetence(getStringFromLUA(L, "getInternalNumber"));

	skills.insert(map<string, lua_State*>::value_type(newSkill, L));
}

void Caracteristiques::manageSkills()
{
	for (map<string, lua_State*>::iterator i = skills.begin() ; i != skills.end() ; ++i)
	{
	}
}

void Caracteristiques::saveSkills(ofstream& stream)
{
	for (map<string, lua_State*>::iterator i = skills.begin() ; i != skills.end() ; ++i)
	{
	}
}

void Caracteristiques::loadSkillFromSavedGame(string skill, string data)
{
}

void Caracteristiques::deleteSkills()
{
	for (map<string, lua_State*>::iterator i = skills.begin() ; i != skills.end() ; )
	{
		lua_State *j = i->second;
		i = skills.erase(i);
		lua_close(j);
	}
}


pair<int, int> Caracteristiques::getFromObjectsAndSkills(string characteristic)
{
	pair<int, int> addedCharacteristic;

	for (auto& i : objects.objects)
	{
		addedCharacteristic.first += getIntFromLUA(i.second, "get" + characteristic);
		addedCharacteristic.second += getIntFromLUA(i.second, "getMult" + characteristic);
	}
	for (auto& i : skills)
	{
		addedCharacteristic.first += getIntFromLUA(i.second, "get" + characteristic);
		addedCharacteristic.second += getIntFromLUA(i.second, "getMult" + characteristic);
	}
	return addedCharacteristic;
}

/** FONCTIONS DE LA CLASSE Activite **/

int Activite::NombreDirections()
{
	int nb = 0;
	for (int a = 0 ; a < 8 ; ++a)
		if (Num_Max[a] != -1) ++nb;

	return nb;
}


/** FONCTIONS DE LA CLASSE Caracteristiques **/

Caracteristiques::~Caracteristiques()
{
	deleteSkills();
}


/** FONCTIONS DE LA CLASSE EmplacementEquipement **/

EmplacementEquipement::~EmplacementEquipement()
{
	delete BoutonEquipement;
	BoutonEquipement = nullptr;
}

void EmplacementEquipement::Set(int x, int y, int w, int h)
{
	if (BoutonEquipement == nullptr) BoutonEquipement = new Bouton;

	if (w == 75 && h == 100)
		BoutonEquipement->Creer(x, y, 75, 100, "FondObjet_75_100", "FondObjet_75_100");
	else if (w == 75 && h == 75)
		BoutonEquipement->Creer(x, y, 75, 75, "FondObjet_75_75", "FondObjet_75_75");
	else if (w == 50 && h == 50)
		BoutonEquipement->Creer(x, y, 50, 50, "FondObjet_50_50", "FondObjet_50_50");
	else
	{
		Erreur("Un emplacement d'équipement a des dimensions non prises en charge", "");
		BoutonEquipement->Creer(x, y, 75, 100, "FondObjet_75_100", "FondObjet_75_100");
	}
}

string EmplacementEquipement::Get_IdEmplacement()
{
	return CategorieObjet + "-" + TypeObjet;
}
