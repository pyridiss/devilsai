
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
#include <tinyxml2.h>

#include "../Bibliotheque/Constantes.h"
#include "../Bibliotheque/luaFunctions.h"
#include "../Bibliotheque/Bibliotheque.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Attributs.h"

#include "tools/filesystem.h"
#include "tools/math.h"

#include "imageManager/imageManager.h"

using namespace tinyxml2;

/* Manager for objects */

Objects::Objects()
  : objects(),
    designs()
{
}

Objects::Objects(const Objects& other)
  : objects(),
    designs(other.designs)
{
    for (auto& i : other.objects)
    {
        WearableItem& o = addObject(i.name(), i.currentSlot());
        o.recoverState(i.currentState());
    }
}

Objects& Objects::operator=(const Objects& right)
{
    designs = right.designs;

    for (auto& i : right.objects)
    {
        WearableItem& o = addObject(i.name(), i.currentSlot());
        o.recoverState(i.currentState());
    }

    return *this;
}

Objects::~Objects()
{
	deleteObjects();
}

WearableItem& Objects::addObject(string newObject, string key, int qualityRandomObject)
{
    WearableItem newItem;
    newItem.create(newObject);
    newItem.generateRandomItem(qualityRandomObject);
    newItem.setSlot(key);

    objects.push_back(std::move(newItem));

    return objects.back();
}

void Objects::deleteObject(const WearableItem& w)
{
    for (auto i = objects.begin() ; i != objects.end() ; ++i)
    {
        if ((*i) == w)
        {
            objects.erase(i);
            break;
        }
    }
}

void Objects::deleteObjects()
{
	objects.clear();
}

WearableItem* Objects::at(const string& slot)
{
    for (auto& i : objects)
        if (i.currentSlot() == slot)
            return &i;

    return nullptr;
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
            WearableItem& o = addObject(design, slot, q);

            if (object->Attribute("data"))
                o.recoverState(object->Attribute("data"));
        }
        if (objectName == "objectDesign")
        {
            string design = object->Attribute("design");
            int q = 0, p = 0;
            object->QueryAttribute("quality", &q);
            object->QueryAttribute("probability", &p);
            designs.emplace_back(design, q, p);
        }
        if (objectName == "createRandomObjects")
        {
            int min = 0, max = 0;
            object->QueryAttribute("min", &min);
            object->QueryAttribute("max", &max);
            int n = tools::math::randomNumber(min, max);

            int designsCount = 0;
            XMLElement* currentDesign = object->FirstChildElement();
            while (currentDesign)
            {
                ++designsCount;
                currentDesign = currentDesign->NextSiblingElement();
            }
            for (int i = 0 ; i < n ; ++i)
            {
                int selectedDesign = rand()%designsCount;
                currentDesign = object->FirstChildElement();
                for (int j = 0 ; j < selectedDesign ; ++j)
                    currentDesign = currentDesign->NextSiblingElement();

                string design = currentDesign->Attribute("design");
                int q = 0;
                currentDesign->QueryAttribute("quality", &q);
                designs.emplace_back(design, q, 100);
            }
            createObjectsFromDesigns();
        }

        object = object->NextSiblingElement();
    }
}

void Objects::createObjectsFromDesigns()
{
    int key = objects.size() + 1;

    for (auto& d : designs)
    {
        if (rand()%100 < d.probability)
        {
            addObject(d.file, "storagebox" + intToString(key, 2), d.quality);
            ++key;
        }
    }
    designs.clear();
}

void Objects::saveToXML(tinyxml2::XMLDocument& doc, tinyxml2::XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    XMLElement* inventory = doc.NewElement("inventory");

    for (auto& o : objects)
    {
        XMLElement* object = doc.NewElement("addObject");

        object->SetAttribute("slot", o.currentSlot().c_str());
        object->SetAttribute("design", o.name().c_str());
        object->SetAttribute("data", o.currentState().c_str());

        inventory->InsertEndChild(object);
    }

    root->InsertEndChild(inventory);
}


/** FONCTIONS DE LA CLASSE Activite **/

Activite::Activite()
  : numberOfImages(0),
    scriptString(),
    interactionField(),
    Animation()
{
}
Activite::Activite(const Activite& other)
  : Id(other.Id),
    numberOfImages(other.numberOfImages),
    speed(other.speed),
    step(other.step),
    fightAtEnd(other.fightAtEnd),
    scriptString(other.scriptString),
    script(other.script),
    speedImprover(other.speedImprover),
    interactionField(other.interactionField),
    priority(other.priority),
    Animation(other.Animation)
{
}

Activite& Activite::operator=(const Activite& right)
{
    Id = right.Id;
    numberOfImages = right.numberOfImages;
    speed = right.speed;
    step = right.step;
    fightAtEnd = right.fightAtEnd;
    scriptString = right.scriptString;
    script = right.script;
    speedImprover = right.speedImprover;
    interactionField = right.interactionField;
    priority = right.priority;
    Animation = right.Animation;

    return *this;
}

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

void Activite::loadFromXML(XMLHandle &handle, Individu* owner)
{
    string ownerName = handle.ToElement()->Attribute("owner");

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

            if (elem->Attribute("speedImprover", "runSpeed"))
                speedImprover = RunSpeed;
            else if (elem->Attribute("speedImprover", "attackSpeed"))
                speedImprover = AttackSpeed;
            else if (elem->Attribute("speedImprover", "injurySpeed"))
                speedImprover = InjurySpeed;
        }
        if (elemName == "interactionField")
        {
            interactionField.loadFromXML(elem);
            if (owner != nullptr)
                interactionField.setOrigin(&owner->position());
        }
        if (elemName == "loadImage")
        {
            string key = elem->Attribute("key");
            string path = elem->Attribute("path");
            imageManager::addImage("skills", key, path);
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
                    string number = intToString(i, 2);
                    path.replace(path.find_first_of('%'), 2, number);
                    string key = ownerName + ":" + Id + "/" + path;
                    imageManager::addImage("individuals", key, path, Vector2i(xAlignment, yAlignment));
                    if (h + s + l != 0)
                        imageManager::changeHSL("individuals", key, h, s, l);

                    addImage(angle * M_PI / 180.0, i, key);
                }
            }

            if (elem->Attribute("imageFile"))
            {
                string path = elem->Attribute("imageFile");
                string key = ownerName + ":" + Id + "/" + path;
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
    lua_register(script, "getElementInteraction", LUA_getTargetedItem);
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

double Activite::damage()
{
    if (script == nullptr)
        return 0;

    lua_getglobal(script, "getDegats");
    lua_call(script, 0, 1);
    double d = lua_tonumber(script, -1);
    lua_pop(script, 1);

    return d;
}

double Activite::amplitude()
{
    if (script == nullptr)
        return 0;

    lua_getglobal(script, "getAmplitude");
    lua_call(script, 0, 1);
    double d = lua_tonumber(script, -1);
    lua_pop(script, 1);

    return d;
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
