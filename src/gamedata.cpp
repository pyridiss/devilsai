
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

#include <thread>
#include <unordered_map>

#include <lua.hpp>
#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/filesystem.h"
#include "tools/signals.h"
#include "tools/math.h"

#include "textManager/textManager.h"
#include "textManager/richText.h"

#include "musicManager/musicManager.h"

#include "devilsai-gui/journal.h"

#include "ElementsCarte/ElementsCarte.h"

#include "Bibliotheque/luaFunctions.h"

#include "devilsai-resources/Carte.h"
#include "devilsai-resources/quests.h"
#include "devilsai-resources/manager.h"
#include "Jeu/options.h"
#include "gamedata.h"

using namespace tinyxml2;

namespace gamedata{

unordered_map<string, lua_State*> _triggersScripts;

Joueur* _player = nullptr;
Carte* _currentWorld = nullptr;
pair<Element_Carte*, string>* _currentPlace = nullptr;

list<textManager::RichText> _listDialogs;
list< pair <string, string> > textFiles;

thread LoadGameThread;

void copyInertItemFromDesign(string t, Paysage *elem)
{
    Paysage* design = devilsai::getResource<Paysage>(t);
    if (design == nullptr)
    {
        tools::debug::error("Cannot initiate inertItem '" + t + "': design does not exist.", "gamedata", __FILENAME__, __LINE__);
        return;
    }

    elem->size = design->size;
    //Restore size origin, as the copy breaks the link.
    elem->size.setOrigin(&elem->position());

	elem->TypeClassement = design->TypeClassement;
    elem->ignoreCollision = design->ignoreCollision;
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

        lua_atpanic(L, [](lua_State* S)
        {
            tools::debug::error(lua_tostring(S, -1), "lua", __FILENAME__, __LINE__);
            return 0;
        });

        lua_register(L, "cout", LUA_cout);
        lua_register(L, "set", LUA_set);
        lua_register(L, "get", LUA_get);
        lua_register(L, "getElement", LUA_getElement);
        lua_register(L, "moveItemTo", LUA_moveItemTo);
        lua_register(L, "addTooltip", LUA_addTooltip);
        lua_register(L, "updateCurrentWorld", [](lua_State* l)
        {
            updateCurrentWorld(lua_tostring(l, 1));
            return 0;
        });
        lua_register(L, "stopPlayer", [](lua_State* l)
        {
            player()->Set_Activite(Behaviors::Random);
            player()->stopAutomoving();
            player()->stopHunting();
            return 0;
        });

        return L;
    }
    else return _triggersScripts.at(name);
}

Joueur* player()
{
    if (_player == nullptr)
        tools::debug::error("No player loaded", "gamedata", __FILENAME__, __LINE__);

    return _player;
}

list<textManager::RichText>& listDialogs()
{
    return _listDialogs;
}

void setPlayerName(string s)
{
    _player->setCustomDisplayedName(textManager::fromStdString(s));
}

Carte* currentWorld()
{
    return _currentWorld;
}

void clear()
{
    for (auto& t : _triggersScripts)
        lua_close(t.second);
    _triggersScripts.clear();

    resources::quests::clear();

    _player = nullptr;
    _currentWorld = nullptr;
    _currentPlace = nullptr;

    _listDialogs.clear();
}

Element_Carte* findElement(int id)
{
    for (auto& w : devilsai::getAllResources<Carte>())
        for (auto& element : w->elements)
            if (element->Id == id) return element;

    return nullptr;
}

Individu* findIndividuUnique(string type)
{
    for (auto& w : devilsai::getAllResources<Carte>())
        for (auto& element : w->elements)
            if (element->Type == type) return dynamic_cast<Individu*>(element);

    return nullptr;
}

list<Element_Carte*> getItemsByTag(const string& w, const string& tag)
{
    list<Element_Carte*> l;

    Carte* w_ptr = devilsai::getResource<Carte>(w);

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

                tools::signals::addSignal("place-changed", textManager::getText("places", p.first->Type).toStdString());
                if (!p.second.empty())
                    musicManager::playMusic(p.second);

                break;
            }
        }
    }
}

void updateCurrentWorld(const string& newWorld)
{
    Carte* w = devilsai::getResource<Carte>(newWorld);

    if (w == nullptr)
    {
        tools::debug::error("Cannot find asked world: " + newWorld, "gamedata", __FILENAME__, __LINE__);
        return;
    }

    _currentWorld = w;

    _currentPlace = nullptr;
    updateCurrentPlace();
}

void saveToXML(XMLDocument& doc, XMLHandle& handle)
{
    XMLElement* root = handle.ToElement();

    for (auto& t : textFiles)
    {
        XMLElement* textElem = doc.NewElement("loadTextFile");
        textElem->SetAttribute("name", t.first.c_str());
        textElem->SetAttribute("file", t.second.c_str());
        root->InsertEndChild(textElem);
    }

    for (auto& w : devilsai::getAllResources<Carte>())
    {
        XMLElement* worldElem = doc.NewElement("loadWorld");
        worldElem->SetAttribute("file", (w->Id + ".xml").c_str());
        worldElem->SetAttribute("name", (w->Id).c_str());
        worldElem->SetAttribute("localFile", true);
        root->InsertEndChild(worldElem);
    }

    XMLElement* properties = doc.NewElement("properties");
    properties->SetAttribute("nextUniqueIdentifier", tools::math::newUniqueIdentifier());
    root->InsertEndChild(properties);

    _player->saveToXML(doc, handle);
    root->LastChildElement()->SetAttribute("world", _currentWorld->Id.c_str());
    root->LastChildElement()->SetAttribute("playerName", _player->displayedName().toStdString().c_str());

    resources::quests::save(doc, root);

    XMLElement* journal = doc.NewElement("journal");
    XMLHandle h(journal);
    saveJournalToXML(doc, h);
    root->InsertEndChild(journal);
}

void loadGameDataFileAsync(const string& dataDirectory, const string& mainFile)
{
    if (!tools::filesystem::checkFile(dataDirectory + mainFile))
    {
        tools::signals::addSignal("unreachable-file");
        tools::signals::addSignal("main-menu");
        return;
    }

    LoadGameThread = thread(loadFromXML, dataDirectory, mainFile);

    thread t([]()
    {
        LoadGameThread.join();
        tools::signals::addSignal("game-started");
    });
    t.detach();
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

        if (elemName == "loadTextFile")
        {
            textManager::loadFile(elem->Attribute("name"), elem->Attribute("file"), options::option<string>(tools::hash("language")));
            textFiles.emplace_back(elem->Attribute("name"), elem->Attribute("file"));
        }

        if (elemName == "species")
        {
            string speciesName = elem->Attribute("name");

            if (devilsai::getResource<Classe_Commune>(speciesName) == nullptr)
            {
                Classe_Commune *s = devilsai::addResource<Classe_Commune>(speciesName);
                XMLHandle hdl2(elem);
                s->loadFromXML(hdl2);
            }
        }

        if (elemName == "inertItemDesign")
        {
            string designName = elem->Attribute("name");

            if (devilsai::getResource<Paysage>(designName) == nullptr)
            {
                Paysage* design = devilsai::addResource<Paysage>(designName);
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

            Carte* w = devilsai::addResource<Carte>(worldName);  // just get it, if it already exists
            w->loadFromFile(dataDirectory + worldFile, tag);
        }

        if ((elemName == "individual") && _player == nullptr)
        {
            if (!elem->Attribute("world"))
            {
                tools::debug::error("Attribute 'world' is not defined in player declaration (" + mainFile + ").", "files", __FILENAME__, __LINE__);
                continue;
            }

            _player = new Joueur;
            _currentWorld = devilsai::getResource<Carte>(elem->Attribute("world"));

            if (_currentWorld == nullptr)
            {
                tools::debug::error("Player is declared in an unknown world: " + string(elem->Attribute("world")), "files", __FILENAME__, __LINE__);
                continue;
            }

            _currentWorld->insertItem(_player);

            XMLHandle playerHandle(elem);
            _player->loadFromXML(playerHandle);

            if (elem->Attribute("playerName"))
                _player->setCustomDisplayedName(textManager::fromStdString(elem->Attribute("playerName")));
        }

        if (elemName == "quest")
        {
            resources::quests::load(elem);
        }

        if (elemName == "journal")
        {
            XMLHandle h(elem);
            loadJournalFromXML(h);
        }

        elem = elem->NextSiblingElement();
    }
}

} //namespace gamedata
