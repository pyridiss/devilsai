
/*
    Devilsai - A game written using the SFML library
    Copyright (C) 2009-2017  Quentin Henriet

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

#include <unordered_map>

#include <lua5.2/lua.hpp>
#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/filesystem.h"
#include "tools/textManager.h"

#include "musicManager/musicManager.h"

#include "Carte/Carte.h"
#include "ElementsCarte/ElementsCarte.h"

#include "Bibliotheque/luaFunctions.h"

#include "gamedata.h"

using namespace tinyxml2;

//Declared in Jeu.h, but will be removed later.
void addQuest(string, string);

namespace gamedata{

unordered_map<string, Carte*> _worlds;
unordered_map<string, Classe_Commune*> _species;
unordered_map<string, Paysage*> _inertItemDesigns;
unordered_map<string, lua_State*> _triggersScripts;
unordered_map<string, lua_State*> _quests;

Joueur* _player = nullptr;
Carte* _currentWorld = nullptr;
pair<Element_Carte*, string>* _currentPlace = nullptr;


void addWorld(const string& id)
{
    auto w = _worlds.emplace(id, new Carte);

    w.first->second->Id = id;

    tools::debug::message("A world has been added: " + id, "gamedata", __FILENAME__, __LINE__);
}

Carte* world(const string& id)
{
    auto i = _worlds.find(id);

    if (i != _worlds.end()) return i->second;

    return nullptr;
}

const unordered_map<string, Carte*>& worlds()
{
    return _worlds;
}

void addSpecies(const string& s)
{
    _species.emplace(s, new Classe_Commune);

    tools::debug::message("A species has been added: " + s, "gamedata", __FILENAME__, __LINE__);
}

Classe_Commune* species(const string& s)
{
    auto i = _species.find(s);

    if (i != _species.end()) return i->second;

    return nullptr;
}

void addInertItemDesign(const string& design)
{
    auto d = _inertItemDesigns.emplace(design, new Paysage);

    d.first->second->Type = design;

    tools::debug::message("An inertItemDesign has been added: " + design, "gamedata", __FILENAME__, __LINE__);
}

Paysage* inertItemDesign(const string& type)
{
    auto i = _inertItemDesigns.find(type);

    if (i != _inertItemDesigns.end()) return i->second;

    return nullptr;
}

void copyInertItemFromDesign(string t, Paysage *elem)
{
    Paysage* design = inertItemDesign(t);
    if (design == nullptr)
    {
        tools::debug::error("Cannot initiate inertItem '" + t + "': design does not exist.", "gamedata", __FILENAME__, __LINE__);
        return;
    }

    elem->size = design->size;
    //Restore size origin, as the copy breaks the link.
    elem->size.setOrigin(&elem->position());

	elem->TypeClassement = design->TypeClassement;
	elem->Diplomatie = 0;
}

lua_State* sharedTrigger(string name)
{
    if (_triggersScripts.find(name) == _triggersScripts.end())
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        luaL_dofile(L, (tools::filesystem::dataDirectory() + name).c_str());
        _triggersScripts.emplace(name, L);

        lua_atpanic(L, LUA_panic);
        lua_register(L, "cout", LUA_cout);
        lua_register(L, "set", LUA_set);
        lua_register(L, "get", LUA_get);

        return L;
    }
    else return _triggersScripts.at(name);
}

unordered_map<string, lua_State*>& quests()
{
    return _quests;
}

Joueur* player()
{
    if (_player == nullptr)
        tools::debug::error("No player loaded", "gamedata", __FILENAME__, __LINE__);

    return _player;
}

void setPlayer(Joueur* p, Carte* w)
{
    _player = p;
    _currentWorld = w;
}

void setPlayerName(string s)
{
    _player->Nom = tools::textManager::fromStdString(s);
}

Carte* currentWorld()
{
    return _currentWorld;
}

void clear()
{
    for (auto& w : _worlds)
        delete w.second;
    _worlds.clear();

    for (auto& s : _species)
        delete s.second;
    _species.clear();

    for (auto& s : _inertItemDesigns)
        delete s.second;
    _inertItemDesigns.clear();

    for (auto& t : _triggersScripts)
        lua_close(t.second);
    _triggersScripts.clear();

    for (auto& q : _quests)
        lua_close(q.second);
    _quests.clear();

    _player = nullptr;
    _currentWorld = nullptr;
    _currentPlace = nullptr;
}

Element_Carte* findElement(int id)
{
    for (auto& w : _worlds)
        for (auto& element : w.second->elements)
            if (element->Id == id) return element;

    return nullptr;
}

Individu_Unique* findIndividuUnique(string type)
{
    for (auto& w : _worlds)
        for (auto& element : w.second->elements)
            if (element->Type == type) return dynamic_cast<Individu_Unique*>(element);

    return nullptr;
}

list<Element_Carte*> getItemsByTag(const string& w, const string& tag)
{
    list<Element_Carte*> l;

    Carte* w_ptr = world(w);

    if (w_ptr != nullptr)
    {
        for (auto& i : w_ptr->elements)
        {
            if (i->Liste == tag)
                l.push_back(i);
        }
    }

    return std::move(l);
}

void updateCurrentPlace()
{
    if (_currentPlace == nullptr || !intersection(_player->size, _currentPlace->first->size))
    {
        for (auto& p : _currentWorld->places)
        {
            if (intersection(p.first->size, _player->size))
            {
                _currentPlace = &p;

                if (p.first->Diplomatie != _player->Diplomatie)
                    tools::signals::addSignal("ingame-toolbar:disable-rest");
                else
                    tools::signals::addSignal("ingame-toolbar:enable-rest");

                tools::signals::addSignal("place-changed", p.first->Type);
                if (!p.second.empty())
                    musicManager::playMusic(p.second);

                break;
            }
        }
    }
}

void saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    for (auto& w : _worlds)
    {
        XMLElement* worldElem = doc.NewElement("loadWorld");
        worldElem->SetAttribute("file", (w.second->Id + ".xml").c_str());
        worldElem->SetAttribute("name", (w.second->Id).c_str());
        worldElem->SetAttribute("localFile", true);
        root->InsertEndChild(worldElem);
    }

    XMLElement* properties = doc.NewElement("properties");
    properties->SetAttribute("nextUniqueIdentifier", tools::math::newUniqueIdentifier());
    root->InsertEndChild(properties);

    _player->saveToXML(doc, handle);
    root->LastChildElement()->SetAttribute("world", _currentWorld->Id.c_str());
    root->LastChildElement()->SetAttribute("playerName", tools::textManager::toStdString(_player->Nom).c_str());

    for (auto& i : _quests)
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

void loadFromXML(const string& dataDirectory, const string& mainFile)
{
    XMLDocument file;
    file.LoadFile((dataDirectory + mainFile).c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "species")
        {
            string speciesName = elem->Attribute("name");

            if (species(speciesName) == nullptr)
            {
                addSpecies(speciesName);
                Classe_Commune *s = species(speciesName);
                XMLHandle hdl2(elem);
                s->loadFromXML(hdl2);
            }
        }

        if (elemName == "inertItemDesign")
        {
            string designName = elem->Attribute("name");

            if (inertItemDesign(designName) == nullptr)
            {
                addInertItemDesign(designName);
                Paysage* design = inertItemDesign(designName);
                XMLHandle hdl2(elem);
                design->loadFromXML(hdl2);
            }
        }

        if (elemName == "loadWorld")
        {
            string worldFile = elem->Attribute("file");
            string worldName = elem->Attribute("name");
            string tag = "ALL";
            if (elem->Attribute("tag")) tag = elem->Attribute("tag");

            Carte* w = world(worldName);
            if (w == nullptr)
            {
                addWorld(worldName);
                w = world(worldName);
            }

            w->loadFromFile(dataDirectory + worldFile, tag);
        }

        if ((elemName == "player" || elemName == "unique") && _player == nullptr)
        {
            if (!elem->Attribute("world"))
            {
                tools::debug::error("Attribute 'world' is not defined in player declaration (" + mainFile + ").", "files", __FILENAME__, __LINE__);
                continue;
            }

            _player = new Joueur;
            _currentWorld = world(elem->Attribute("world"));

            if (currentWorld == nullptr)
            {
                tools::debug::error("Player is declared in an unknown world: " + string(elem->Attribute("world")), "files", __FILENAME__, __LINE__);
                continue;
            }

            _currentWorld->elements.push_back(_player);

            XMLHandle playerHandle(elem);
            _player->loadFromXML(playerHandle);

            if (elem->Attribute("playerName"))
                _player->Nom = tools::textManager::fromStdString(elem->Attribute("playerName"));
        }

        if (elemName == "quest")
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
                auto i = _quests.find(questFile);
                if (i != _quests.end())
                {
                    lua_getglobal(i->second, "questRecoverState");
                    lua_pushstring(i->second, currentState.c_str());
                    lua_call(i->second, 1, 0);
                }
                else tools::debug::error("Error while loading quest " + questFile, "files", __FILENAME__, __LINE__);
            }
        }

        //Just to make old maps work, will be removed later

        if (elemName == "CARTE")
        {
            string n = elem->Attribute("number");
            Load_Carte(n, TYPE_CARTE);
        }
        if (elemName == "LISTE")
        {
            string n = elem->Attribute("number");
            Load_Carte(n, TYPE_LISTE);
        }

        elem = elem->NextSiblingElement();
    }
}

} //namespace gamedata
