
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

#include <tinyxml2.h>

#include "tools/filesystem.h"

#include "../Carte/Carte.h"

#include "gamedata.h"
#include "options.h"

using namespace tinyxml2;

void Save_Partie()
{
    options::updateCurrentSavedGamePack();

    string path = tools::filesystem::getSaveDirectoryPath() + options::getCurrentSavedGameDirectory();

    //'files' will store the filenames to create index.xml
    list<string> files;


    //gamedata.xml
    XMLDocument gamedata_xml;

    XMLElement* gamedata_xml_root = gamedata_xml.NewElement("gamedata");
    gamedata_xml.InsertFirstChild(gamedata_xml_root);

    XMLHandle gamedata_xml_handle(gamedata_xml_root);
    gamedata::saveToXML(gamedata_xml, gamedata_xml_handle);

    gamedata_xml.SaveFile((path + "gamedata.xml").c_str());
    files.push_back("gamedata.xml");


    //worlds xml files
    const auto& worlds = gamedata::worlds();

    for (const auto& w : worlds)
    {
        XMLDocument world_xml;

        XMLElement* world_xml_root = world_xml.NewElement("world");
        world_xml.InsertFirstChild(world_xml_root);

        XMLHandle world_xml_handle(world_xml_root);
        w.second->saveToXML(world_xml, world_xml_handle);

        world_xml.SaveFile((path + w.second->Id + ".xml").c_str());
        files.push_back(w.second->Id + ".xml");
    }


    //index.xml
    XMLDocument index_xml;

    XMLElement* index_xml_root = index_xml.NewElement("files");
    index_xml.InsertFirstChild(index_xml_root);

    for (auto& f : files)
    {
        XMLElement* fileName = index_xml.NewElement("file");
        fileName->SetAttribute("path", f.c_str());
        index_xml_root->InsertEndChild(fileName);
    }

    index_xml.SaveFile((path + "index.xml").c_str());
}

bool Load_Partie(string path)
{
    gamedata::loadFromXML(tools::filesystem::getSaveDirectoryPath() + path, "gamedata.xml");
    options::updateCurrentSavedGamePack(path);
}
