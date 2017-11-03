
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

#include <list>
#include <unordered_map>

#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/variant.h"
#include "tools/signals.h"
#include "tools/filesystem.h"
#include "tools/textManager.h"

#include "../Bibliotheque/Bibliotheque.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"

#include "gui/window.h"
#include "gui/scrollingList.h"

#include "gamedata.h"

#include "options.h"

using namespace tinyxml2;

namespace options{

struct SavedGame
{
    string directory;
    String32 playerName;
    string version;

    bool operator==(const SavedGame& right)
    {
        if (directory == right.directory)
            return true;
        return false;
    }
};

typedef tools::math::Variant<unsigned, bool, string> optionType;

list<SavedGame> savedGames;
SavedGame* currentSavedGame = nullptr;
int nextGameNumber = 1;
unordered_map<string, optionType> _options;


template<typename T>
void addOption(const string& name, T value)
{
    optionType o;
    o.set<T>(value);

    const auto& i = _options.find(name);
    if (i == _options.end())
        _options.emplace(name, o);
    else
        i->second = o;
}

//Explicit instantiations for the linker
template void addOption<unsigned>(const string& name, unsigned value);
template void addOption<bool>(const string& name, bool value);
template void addOption<string>(const string& name, string value);

template<typename T>
T option(const string& name)
{
    const auto& i = _options.find(name);
    if (i != _options.end())
        return i->second.get<T>();

    tools::debug::error("This option does not exist: " + name, "options", __FILENAME__, __LINE__);
    return T();
}

//Explicit instantiations for the linker
template unsigned option<unsigned>(const string& name);
template bool option<bool>(const string& name);
template string option<string>(const string& name);


void Load_Options()
{
    string path = tools::filesystem::getSaveDirectoryPath() + "options.xml";

    ifstream Fichier(path);
    if (!Fichier.good())
        path = tools::filesystem::dataDirectory() + "defaut.opt";

    XMLDocument file;
    file.LoadFile(path.c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "configuration")
        {
            Options.Langue = elem->Attribute("language");
            elem->QueryAttribute("console", &Options.AffichageDegats);
            elem->QueryAttribute("next-game-number", &nextGameNumber);
        }
        else if (elemName == "options")
        {
            XMLHandle hdl2(elem);
            XMLElement *elem2 = hdl2.FirstChildElement().ToElement();
            while (elem2)
            {
                string elem2Name = elem2->Name();
                if (elem2Name == "option")
                {
                    string name = elem2->Attribute("name");
                    string type = elem2->Attribute("type");
                    if (type == "boolean")
                    {
                        bool b = false;
                        elem2->QueryAttribute("value", &b);
                        addOption<bool>(name, b);
                    }
                    else if (type == "unsigned")
                    {
                        unsigned u = 0;
                        elem2->QueryAttribute("value", &u);
                        addOption<unsigned>(name, u);
                    }
                    else if (type == "string")
                    {
                        string s = elem2->Attribute("value");
                        addOption<string>(name, s);
                    }
                }

                elem2 = elem2->NextSiblingElement();
            }
        }
        else if (elemName == "savedGame")
        {
            SavedGame s;
            s.directory = elem->Attribute("directory");
            s.version = elem->Attribute("version");
            string n = elem->Attribute("playerName");
            s.playerName = tools::textManager::fromStdString(n);
            savedGames.push_back(std::move(s));
        }
        elem = elem->NextSiblingElement();
    }

    Options.ScreenW = option<unsigned>("screen-width");
    Options.ScreenH = option<unsigned>("screen-height");
}

void Save_Options()
{
    XMLDocument file;

    XMLElement* elem = file.NewElement("options");
    file.InsertFirstChild(elem);

    XMLElement* config = file.NewElement("configuration");
    config->SetAttribute("language", Options.Langue.c_str());
    config->SetAttribute("console", Options.AffichageDegats);
    config->SetAttribute("next-game-number", nextGameNumber);
    elem->InsertEndChild(config);

    XMLElement* options = file.NewElement("options");
    for (auto& o : _options)
    {
        XMLElement* option = file.NewElement("option");
        option->SetAttribute("name", o.first.c_str());
        if (o.second.is<bool>())
        {
            option->SetAttribute("type", "boolean");
            option->SetAttribute("value", o.second.get<bool>());
        }
        else if (o.second.is<unsigned>())
        {
            option->SetAttribute("type", "unsigned");
            option->SetAttribute("value", o.second.get<unsigned>());
        }
        else if (o.second.is<string>())
        {
            option->SetAttribute("type", "string");
            option->SetAttribute("value", o.second.get<string>().c_str());
        }
        options->InsertEndChild(option);
    }
    elem->InsertEndChild(options);

    for (auto& s : savedGames)
    {
        XMLElement* savedGame = file.NewElement("savedGame");
        savedGame->SetAttribute("directory", s.directory.c_str());
        savedGame->SetAttribute("version", s.version.c_str());
        savedGame->SetAttribute("playerName", tools::textManager::toStdString(s.playerName).c_str());
        elem->InsertEndChild(savedGame);
    }

    file.SaveFile((tools::filesystem::getSaveDirectoryPath() + "options.xml").c_str());
}

void changeOption(string option, string value)
{
    if (option == "option-change-language")
    {
        if (value == "EN") Options.Langue = "en=";
        if (value == "FR") Options.Langue = "fr=";
    }
    else if (option == "option-change-resolution")
    {
        size_t x = value.find("x");
        Options.ScreenW = tools::textManager::toInt(value);
        Options.ScreenH = tools::textManager::toInt(value.substr(x+1));
        addOption<unsigned>("screen-width", Options.ScreenW);
        addOption<unsigned>("screen-height", Options.ScreenH);
        createWindow();
    }
    else if (option == "option-change-fullscreen")
    {
        addOption<bool>("screen-fullscreen", (value == "enabled" ? true : false));
        createWindow();
    }
    else if (option == "option-change-console")
    {
        Options.AffichageDegats = (value == "enabled" ? true : false);
    }
}

void createNewSavedGamePack()
{
    SavedGame s;

    s.directory = intToString(nextGameNumber, 4) + "/";
    s.playerName = gamedata::player()->Nom;
    s.version = "master";

    tools::filesystem::createDirectory(tools::filesystem::getSaveDirectoryPath() + s.directory);

    savedGames.push_back(std::move(s));
    ++nextGameNumber;

    currentSavedGame = &savedGames.back();
    Save_Options();
}

void updateCurrentSavedGamePack(string directory)
{
    if (!directory.empty())
    {
        for (auto& s : savedGames)
        {
            if (s.directory == directory)
                currentSavedGame = &s;
        }
    }

    if (currentSavedGame == nullptr)
        createNewSavedGamePack();

    currentSavedGame->version = "master";
}

void deleteSavedGamePack(string directory)
{
    string path = tools::filesystem::getSaveDirectoryPath() + directory;

    XMLDocument file;
    file.LoadFile((path + "index.xml").c_str());

    XMLHandle hdl(file);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "file")
        {
            string file = elem->Attribute("path");
            tools::filesystem::removeFile(path + file);
        }

        elem = elem->NextSiblingElement();
    }

    tools::filesystem::removeFile(path + "index.xml");

    tools::filesystem::removeDirectory(path);

    for (auto& s : savedGames)
    {
        if (directory == s.directory)
        {
            savedGames.remove(s);
            break;
        }
    }
}

string getCurrentSavedGameDirectory()
{
    if (currentSavedGame != nullptr)
        return currentSavedGame->directory;

    return "";
}

void initLoadGameWindow(gui::Window& window)
{
    gui::ScrollingList* scrollingList = dynamic_cast<gui::ScrollingList*>(window.widget("savedgames-list"));

    scrollingList->removeAllEntries();

    if (scrollingList != nullptr)
    {
        for (auto& s : savedGames)
        {
            scrollingList->addEntry(s.playerName, s.directory);
        }
    }

    if (!savedGames.empty())
        tools::signals::addSignal("main-menu:enable-load-game");
}

void initOptionsWindow(gui::Window& window)
{
    string d;

    if (Options.Langue == "fr=") d = "FR";
    else d = "EN";
    window.setValue("chooser-language", d);

    d = intToString(Options.ScreenW) + "x" + intToString(Options.ScreenH);
    window.setValue("chooser-resolution", d);

    d = (option<bool>("screen-fullscreen") ? "enabled" : "disabled");
    window.setValue("chooser-fullscreen", d);

    d = (Options.AffichageDegats ? "enabled" : "disabled");
    window.setValue("chooser-console", d);
}

} //namespace options
