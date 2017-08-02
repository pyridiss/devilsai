
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

#include "tools/filesystem.h"

#include "imageManager/imageManager.h"

using namespace tinyxml2;

/* Manager for objects */

Objects::~Objects()
{
	deleteObjects();
}

lua_State* Objects::addObject(string newObject, string key, int qualityRandomObject)
{
	MESSAGE("Chargement du l'objet " + newObject + "…", FICHIER)

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	luaL_dofile(L, (tools::filesystem::dataDirectory() + "object/" + newObject + ".lua").c_str());

	lua_atpanic(L, LUA_panic);

	lua_register(L, "cout", LUA_cout);

	if (qualityRandomObject > 0) setIntToLUA(L, "generateRandomObject", qualityRandomObject);

    imageManager::addContainer("objects");
    imageManager::addContainer("objectsIcons");

    imageManager::addImage("objects", newObject, getStringFromLUA(L, "getImageFile"));
    imageManager::addImage("objectsIcons", newObject, getStringFromLUA(L, "getIconFile"));

	pair<mapObjects::iterator, bool> result = objects.insert(mapObjects::value_type(key, L));

	if (result.second) setStringToLUA(L, "setKey", key);
	else
	{
		Erreur("Impossible de créer l'objet demandé :", newObject + " / " + key);
		MESSAGE("… FAIL", FICHIER)
		lua_close(L);
	}

	MESSAGE("… OK", FICHIER)
    return result.first->second;
}

void Objects::deleteObject(lua_State* obj)
{
	lua_close(obj);
	obj = NULL;
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

void Objects::loadFromXML(XMLElement* elem)
{
    deleteObjects();

    XMLHandle hdl(elem);
    XMLElement *object = hdl.FirstChildElement().ToElement();
    while (object)
    {
        string objectName = object->Name();

        if (objectName == "addObject")
        {
            string slot = object->Attribute("slot");
            string design = object->Attribute("design");
            int q = 0;
            object->QueryAttribute("quality", &q);
            lua_State* L = addObject(design, slot, q);

            if (object->Attribute("data"))
            {
                lua_getglobal(L, "objectRecoverState");
                lua_pushstring(L, object->Attribute("data"));
                lua_call(L, 1, 0);
            }
        }

        object = object->NextSiblingElement();
    }

}

void Objects::saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* inventory = doc.NewElement("inventory");

    for (auto& o : objects)
    {
        XMLElement* object = doc.NewElement("addObject");

        object->SetAttribute("slot", o.first.c_str());

        lua_getglobal(o.second, "getFileName");
        lua_call(o.second, 0, 1);
        object->SetAttribute("design", lua_tostring(o.second, -1));

        lua_getglobal(o.second, "objectSave");
        lua_call(o.second, 0, 1);
        object->SetAttribute("data", lua_tostring(o.second, -1));

        inventory->InsertEndChild(object);
    }

    root->InsertEndChild(inventory);
}

Caracteristiques::Caracteristiques() :
    strength(0), power(0), agility(0), intellect(0),
    constitution(0), charisma(0), dodge(0), healingPower(0),
    runSpeed(0), attackSpeed(0), injurySpeed(0)
{
}

void Caracteristiques::loadFromXML(tinyxml2::XMLElement* elem)
{
    elem->QueryAttribute("strength", &strength);
    elem->QueryAttribute("power", &power);
    elem->QueryAttribute("agility", &agility);
    elem->QueryAttribute("intellect", &intellect);
    elem->QueryAttribute("constitution", &constitution);
    elem->QueryAttribute("charisma", &charisma);
    elem->QueryAttribute("dodge", &dodge);
    elem->QueryAttribute("healingPower", &healingPower);
    elem->QueryAttribute("runSpeed", &runSpeed);
    elem->QueryAttribute("attackSpeed", &attackSpeed);
    elem->QueryAttribute("injurySpeed", &injurySpeed);
}

void Caracteristiques::saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    root->SetAttribute("strength", strength);
    root->SetAttribute("power", power);
    root->SetAttribute("agility", agility);
    root->SetAttribute("intellect", intellect);
    root->SetAttribute("constitution", constitution);
    root->SetAttribute("charisma", charisma);
    root->SetAttribute("dodge", dodge);
    root->SetAttribute("healingPower", healingPower);
    root->SetAttribute("runSpeed", runSpeed);
    root->SetAttribute("attackSpeed", attackSpeed);
    root->SetAttribute("injurySpeed", injurySpeed);
}

pair<int, int> Caracteristiques::getFromObjectsAndSkills(string characteristic)
{
	pair<int, int> addedCharacteristic;

	return addedCharacteristic;
}

double Caracteristiques::operator[](string characteristic) const
{
	if (characteristic == "strength")
        return strength;
	if (characteristic == "power")
        return power;
	if (characteristic == "agility")
        return agility;
	if (characteristic == "intellect")
        return intellect;

	if (characteristic == "constitution")
        return constitution;
	if (characteristic == "charisma")
        return charisma;
	if (characteristic == "dodge")
        return dodge;
	if (characteristic == "healingPower")
        return healingPower;

	return Jeu.intNotFound;
}

double Caracteristiques::operator[](Attribute a)
{
    return get(a);
}

void Caracteristiques::add(Attribute a, double value)
{
    get(a) += value;
}

void Caracteristiques::set(Attribute a, double value)
{
    get(a) = value;
}

double& Caracteristiques::get(Attribute a)
{
    switch (a)
    {
        case Strength: return strength;
        case Power: return power;
        case Agility: return agility;
        case Intellect: return intellect;
        case Constitution: return constitution;
        case Charisma: return charisma;
        case Dodge: return dodge;
        case HealingPower: return healingPower;
        case RunSpeed: return runSpeed;
        case AttackSpeed: return attackSpeed;
        case InjurySpeed: return injurySpeed;
    }
}

string Caracteristiques::toString(Attribute a)
{
    switch (a)
    {
        case Strength: return "strength";
        case Power: return "power";
        case Agility: return "agility";
        case Intellect: return "intellect";
        case Constitution: return "constitution";
        case Charisma: return "charisma";
        case Dodge: return "dodge";
        case HealingPower: return "healingPower";
        case RunSpeed: return "runSpeed";
        case AttackSpeed: return "attackSpeed";
        case InjurySpeed: return "injurySpeed";
    }
}

double Statistiques::operator[](string stat) const
{
	if (stat == "life" || stat == "vitality" || stat == "Vitalite")
        return Vitalite;
	if (stat == "energy" || stat == "Energie")
        return Energie;
	if (stat == "healing")
        return Recuperation;
}

double Statistiques::operator[](Attribute a)
{
    return get(a);
}

void Statistiques::add(Attribute a, double value)
{
    get(a) += value;
}

void Statistiques::set(Attribute a, double value)
{
    get(a) = value;
}

double& Statistiques::get(Attribute a)
{
    switch (a)
    {
        case Life: return Vitalite;
        case Energy: return Energie;
        case Healing: return Recuperation;
    }
}

string Statistiques::toString(Attribute a)
{
    switch (a)
    {
        case Life: return "life";
        case Energy: return "energy";
        case Healing: return "healing";
    }
}

void Statistiques::loadFromXML(tinyxml2::XMLElement* elem)
{
    elem->QueryAttribute("vitality", &Vitalite);
    elem->QueryAttribute("energy", &Energie);
    elem->QueryAttribute("healing", &Recuperation);
}

void Statistiques::saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    root->SetAttribute("vitality", Vitalite);
    root->SetAttribute("energy", Energie);
    root->SetAttribute("healing", Recuperation);
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
    string owner = handle.ToElement()->Attribute("owner");

    XMLElement *elem = handle.FirstChildElement().ToElement();

    int h = 0, s = 0, l = 0;

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
            elem->QueryAttribute("hue", &h);
            elem->QueryAttribute("saturation", &s);
            elem->QueryAttribute("luminance", &l);
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
                    string key = owner + ":" + Id + "/" + path;
                    imageManager::addImage("individuals", key, path, Vector2i(xAlignment, yAlignment));
                    if (h + s + l != 0)
                        imageManager::changeHSL("individuals", key, h, s, l);

                    addImage(angle * M_PI / 180.0, i, key);
                }
            }

            if (elem->Attribute("imageFile"))
            {
                string path = elem->Attribute("imageFile");
                string key = owner + ":" + Id + "/" + path;
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

    lua_register(script, "addSound", LUA_addSound);
    lua_register(script, "cout", LUA_cout);
    lua_register(script, "createIndividual", LUA_createIndividual);
    lua_register(script, "fight", LUA_combat);
    lua_register(script, "getElementInteraction", LUA_getElementInteraction);
    lua_register(script, "individual_changeCurrentSkill", LUA_changeCurrentSkill);
    lua_register(script, "individual_copy", LUA_individual_copy);
    lua_register(script, "individual_get", LUA_get);
    lua_register(script, "individual_set", LUA_set);
    lua_register(script, "isIndividu", LUA_isIndividu);
    lua_register(script, "playSound", LUA_playSound);
    lua_register(script, "useObject", LUA_useObject);

    luaL_loadbuffer(script, scriptString.c_str(), scriptString.length(), Id.c_str());
    lua_pcall(script, 0, 0, 0);
}

void Activite::atBegin(Individu* owner)
{
    if (script == nullptr)
        return;

    lua_getglobal(script, "atBegin");
    lua_pushlightuserdata(script, (void*)owner);
    lua_call(script, 1, 0);
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
}
