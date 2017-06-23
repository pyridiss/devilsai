
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

#include <cmath>

#include <lua5.2/lua.hpp>
#include <tinyxml2.h>

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/luaFunctions.h"
#include "../Jeu/Jeu.h"
#include "Attributs.h"

#include "imageManager/imageManager.h"

using namespace tinyxml2;

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

	luaL_dofile(L, (INSTALL_DIR + "object/" + newObject + ".lua").c_str());

	lua_atpanic(L, LUA_panic);

	lua_register(L, "cout", LUA_cout);

	if (qualityRandomObject > 0) setIntToLUA(L, "generateRandomObject", qualityRandomObject);

    imageManager::addContainer("objects");
    imageManager::addContainer("objectsIcons");

    imageManager::addImage("objects", newObject, INSTALL_DIR + getStringFromLUA(L, "getImageFile"));
    imageManager::addImage("objectsIcons", newObject, INSTALL_DIR + getStringFromLUA(L, "getIconFile"));

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

void Caracteristiques::addSkill(string newSkill, Individu* owner)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dofile(L, (INSTALL_DIR + "skill/" + newSkill + ".lua").c_str());

	lua_atpanic(L, LUA_panic);

	lua_register(L, "addImageProjectile", [](lua_State* L) {
        string path = INSTALL_DIR + lua_tostring(L, 2);
        imageManager::addContainer("projectiles");
        imageManager::addImage("projectiles", lua_tostring(L, 1), path);
        return 0;
	});

	lua_register(L, "getElementById", LUA_getElementById);
	lua_register(L, "getElementInteraction", LUA_getElementInteraction);
	lua_register(L, "isIndividu", LUA_isIndividu);
	lua_register(L, "interact", LUA_interact);
	lua_register(L, "combat", LUA_combat);
	lua_register(L, "createProjectile", LUA_createProjectile);
	lua_register(L, "set", LUA_set);
	lua_register(L, "get", LUA_get);
	lua_register(L, "useObject", LUA_useObject);
	lua_register(L, "dispText", LUA_dispText);
	lua_register(L, "getQuantityOf", LUA_getQuantityOf);
	lua_register(L, "I", LUA_I);
	lua_register(L, "cout", LUA_cout);
	lua_register(L, "createActivite", LUA_createActivite);
	lua_register(L, "activiteSet", LUA_activiteSet);
	lua_register(L, "activiteAddImage", LUA_activiteAddImage);
	lua_register(L, "addSound", LUA_addSound);
	lua_register(L, "playSound", LUA_playSound);
    lua_register(L, "individual_get", LUA_get);
    lua_register(L, "individual_set", LUA_set);
    lua_register(L, "individual_copy", LUA_individual_copy);
    lua_register(L, "createIndividual", LUA_createIndividual);

	lua_getglobal(L, "skillBegin");
    lua_pushlightuserdata(L, (void*)owner);
	lua_call(L, 1, 0);

    string name = getStringFromLUA(L, "getName");
    string file = INSTALL_DIR + getStringFromLUA(L, "getImageFile");

    imageManager::addContainer("skills");
    imageManager::addImage("skills", name, file);

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

int& Caracteristiques::operator[](string characteristic)
{
	if (characteristic == "Force")
		return Force;
	if (characteristic == "Puissance")
		return Puissance;
	if (characteristic == "Agilite")
		return Agilite;
	if (characteristic == "Intelligence")
		return Intelligence;

	if (characteristic == "Constitution")
		return Constitution;
	if (characteristic == "Charisme")
		return Charisme;
	if (characteristic == "Esquive")
		return Esquive;
	if (characteristic == "RecuperationMoyenne")
		return RecuperationMoyenne;

	return Jeu.intNotFound;
}

const int& Caracteristiques::operator[](string characteristic) const
{
	if (characteristic == "Force")
		return Force;
	if (characteristic == "Puissance")
		return Puissance;
	if (characteristic == "Agilite")
		return Agilite;
	if (characteristic == "Intelligence")
		return Intelligence;

	if (characteristic == "Constitution")
		return Constitution;
	if (characteristic == "Charisme")
		return Charisme;
	if (characteristic == "Esquive")
		return Esquive;
	if (characteristic == "RecuperationMoyenne")
		return RecuperationMoyenne;

	return Jeu.intNotFound;
}

float& Statistiques::operator[](string stat)
{
	if (stat == "Vitalite" || stat == "vitality")
		return Vitalite;
	if (stat == "Energie" || stat == "energy")
		return Energie;
	if (stat == "Recuperation" || stat == "recuperation")
		return Recuperation;

	return Jeu.floatNotFound;
}

const float& Statistiques::operator[](string stat) const
{
	if (stat == "Vitalite" || stat == "vitality")
		return Vitalite;
	if (stat == "Energie" || stat == "energy")
		return Energie;
	if (stat == "Recuperation" || stat == "recuperation")
		return Recuperation;

	return Jeu.floatNotFound;
}


/** FONCTIONS DE LA CLASSE Activite **/

Activite::~Activite()
{
    if (script != nullptr) lua_close(script);
}

void Activite::addImage(double angle, int num, string imageKey)
{
    pair<double, int> key(angle, num);
    Animation.insert(pair<pair<double,int>,string>(key, imageKey));

    if (angle == 0) addImage(2.0 * M_PI, num, imageKey);
}

string Activite::getImageKey(double angle, int num)
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

void Activite::loadFromXML(XMLHandle &handle)
{
    XMLElement *elem = handle.FirstChildElement().ToElement();
    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "properties")
        {
            elem->QueryAttribute("priority", &priority);
            elem->QueryAttribute("speed", &speed);
            elem->QueryAttribute("step", &step);
            elem->QueryAttribute("fightAtEnd", &fightAtEnd); //TODO
            elem->QueryAttribute("numberOfImages", &numberOfImages);
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

                for (int i = 0 ; i < numberOfImages ; ++i)
                {
                    string path = pathPattern;
                    string number = intToString(i, 2);
                    path.replace(path.find_first_of('%'), 2, number);
                    string key = "activity" + Id + ":" + path; //TODO
                    imageManager::addImage("individuals", key, path, Vector2i(xAlignment, yAlignment));

                    addImage(angle * M_PI / 180.0, i, key);
                }
            }

            if (elem->Attribute("imageFile"))
            {
                string path = elem->Attribute("imageFile");
                path = INSTALL_DIR + path;
                string key = "activity" + Id + ":" + path; //TODO
                imageManager::addImage("individuals", key, path, Vector2i(xAlignment, yAlignment));
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

void Activite::loadScript()
{
    script = luaL_newstate();
    luaL_openlibs(script);

    lua_atpanic(script, LUA_panic);

    lua_register(script, "cout", LUA_cout);
    lua_register(script, "individual_get", LUA_get);
    lua_register(script, "individual_set", LUA_set);
    lua_register(script, "individual_copy", LUA_individual_copy);
    lua_register(script, "createIndividual", LUA_createIndividual);

    luaL_loadbuffer(script, scriptString.c_str(), scriptString.length(), Id.c_str());
    lua_pcall(script, 0, 0, 0);
}

void Activite::atEnd(Individu* owner)
{
    if (script == nullptr)
        return;

    lua_getglobal(script, "atEnd");
    lua_pushlightuserdata(script, (void*)owner);
    lua_call(script, 1, 0);
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
    if (BoutonEquipement == nullptr) BoutonEquipement = new gui::Button;

    BoutonEquipement->setTopLeftCoordinates(x, y);
    BoutonEquipement->setSize(w, h);

    if (w == 75 && h == 100)
        BoutonEquipement->setAllBackground("FondObjet_75_100");
    else if (w == 75 && h == 75)
        BoutonEquipement->setAllBackground("FondObjet_75_75");
    else if (w == 50 && h == 50)
        BoutonEquipement->setAllBackground("FondObjet_50_50");
    else
    {
        Erreur("Un emplacement d'équipement a des dimensions non prises en charge", "");
        BoutonEquipement->setAllBackground("FondObjet_75_100");
    }

    BoutonEquipement->setForegroundShader("hover", gui::style::highlightShader);
}

string EmplacementEquipement::Get_IdEmplacement()
{
	return CategorieObjet + "-" + TypeObjet;
}
