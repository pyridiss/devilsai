
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

#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/filesystem.h"

#include "Carte/Carte.h"
#include "ElementsCarte/ElementsCarte.h"

#include "gamedata.h"

using namespace tinyxml2;

namespace gamedata{

unordered_map<string, Carte*> _worlds;
unordered_map<string, Classe_Commune*> _species;
unordered_map<string, Paysage*> _inertItemDesigns;

Joueur* _player = nullptr;
Carte* _currentWorld = nullptr;
Element_Carte* _currentPlace = nullptr;


void addWorld(const string& id)
{
    auto w = _worlds.emplace(id, new Carte);

    w.first->second->Id = id;

    tools::debug::message("A world has been added: " + id, "gamedata");
}

void clearWorlds()
{
    for (auto& w : _worlds)
        delete w.second;

    _worlds.clear();

    tools::debug::message("Worlds list has been cleared.", "gamedata");
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

    tools::debug::message("A species has been added: " + s, "gamedata");
}

void clearSpecies()
{
    for (auto& s : _species)
        delete s.second;

    _species.clear();

    tools::debug::message("Species list has been cleared.", "gamedata");
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

    tools::debug::message("An inertItemDesign has been added: " + design, "gamedata");
}

void clearInertItemDesigns()
{
    for (auto& s : _inertItemDesigns)
        delete s.second;

    _inertItemDesigns.clear();

    tools::debug::message("inertItemDesigns list has been cleared.", "gamedata");
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
        tools::debug::error("Cannot initiate inertItem '" + t + "': design does not exist.", "gamedata");
        return;
    }

    elem->size = design->size;
    //Restore size origin, as the copy breaks the link.
    elem->size.setOrigin(&elem->position());

	elem->TypeClassement = design->TypeClassement;
	elem->Diplomatie = 0;
}

Joueur* player()
{
    if (_player == nullptr)
        tools::debug::error("No player loaded", "gamedata");

    return _player;
}

void setPlayer(Joueur* p, Carte* w)
{
    _player = p;
    _currentWorld = w;
}

Carte* currentWorld()
{
    return _currentWorld;
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

void updateCurrentPlace()
{
    if (_currentPlace == nullptr || !intersection(_player->size, _currentPlace->size))
    {
        for (auto& p : _currentWorld->places)
        {
            if (intersection(p->size, _player->size))
            {
                _currentPlace = p;

                if (p->Diplomatie != _player->Diplomatie)
                    tools::signals::addSignal("ingame-toolbar:disable-rest");
                else
                    tools::signals::addSignal("ingame-toolbar:enable-rest");

                tools::signals::addSignal("place-changed", p->Type);

                break;
            }
        }
    }
}

void saveToXML(XMLDocument& doc, XMLHandle& handle)
{
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

        if (elemName == "uniqueData")
        {
            string uniqueName = elem->Attribute("name");

            Individu_Unique* u = findIndividuUnique(uniqueName);
            if (u != nullptr)
            {
                XMLHandle hdl2(elem);
                u->loadFromXML(hdl2);
            }
        }

        if (elemName == "player" || elemName == "unique")
        {
            string w = elem->Attribute("world");

            if (_player == nullptr)
            {
                _player = new Joueur;
                _currentWorld = world(w);
                _currentWorld->elements.push_back(_player);
                XMLHandle playerHandle(elem);
                _player->loadFromXML(playerHandle);
            }
        }

        elem = elem->NextSiblingElement();
    }
}

} //namespace gamedata
