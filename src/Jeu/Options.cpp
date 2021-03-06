
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
#include <fstream>

#include <tinyxml2.h>

#include "tools/debug.h"
#include "tools/variant.h"
#include "tools/signals.h"
#include "tools/filesystem.h"
#include "tools/math.h"
#include "textManager/textManager.h"

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
    textManager::PlainText playerName;
    string version;

    SavedGame()
      : directory(),
        playerName(),
        version()
    {
    }
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
unordered_map<unsigned int, optionType> _options;


template<typename T>
void addOption(unsigned int hash, T value)
{
    const auto& i = _options.find(hash);

    if (i == _options.end())
    {
        optionType o;
        o.set<T>(value);
        _options.emplace(hash, o);
    }
    else
        i->second.set<T>(value);
}

template<typename T>
void addOption(string_view name, T value)
{
    addOption<T>(tools::hash(name.data()), value);
}

//Explicit instantiations for the linker
template void addOption<unsigned>(unsigned int hash, unsigned value);
template void addOption<bool>(unsigned int hash, bool value);
template void addOption<string>(unsigned int hash, string value);

template<typename T>
T option(unsigned int hash)
{
    const auto& i = _options.find(hash);
    if (i != _options.end())
        return i->second.get<T>();

    tools::debug::error("This option does not exist: " + textManager::toString(hash), "options", __FILENAME__, __LINE__);
    return T();
}

template<typename T>
T option(string_view name)
{
    return option<T>(tools::hash(name.data()));
}

//Explicit instantiations for the linker
template unsigned option<unsigned>(unsigned int hash);
template bool option<bool>(unsigned int hash);
template string option<string>(unsigned int hash);


void loadFromFile(const string& file)
{
    if (!tools::filesystem::checkFile(file))
        return;

    XMLDocument doc;
    doc.LoadFile(file.c_str());

    XMLHandle hdl(doc);
    XMLElement *elem = hdl.FirstChildElement().FirstChildElement().ToElement();

    while (elem)
    {
        string elemName = elem->Name();

        if (elemName == "option")
        {
            string_view name = elem->Attribute("name");
            unsigned int hash = 0;
            elem->QueryAttribute("hash", &hash);

            string_view type = elem->Attribute("type");
            if (type == "boolean")
            {
                bool b = false;
                elem->QueryAttribute("value", &b);
                if (name.size() > 0)
                    addOption<bool>(name, b);
                else if (hash != 0)
                    addOption<bool>(hash, b);
            }
            else if (type == "unsigned")
            {
                unsigned u = 0;
                elem->QueryAttribute("value", &u);
                if (name.size() > 0)
                    addOption<unsigned>(name, u);
                else if (hash != 0)
                    addOption<unsigned>(hash, u);
            }
            else if (type == "string")
            {
                string s = elem->Attribute("value");
                if (name.size() > 0)
                    addOption<string>(name, s);
                else if (hash != 0)
                    addOption<string>(hash, s);
            }
        }
        else if (elemName == "savedGame")
        {
            SavedGame s;
            s.directory = elem->Attribute("directory");
            s.version = elem->Attribute("version");
            string n = elem->Attribute("playerName");
            s.playerName = textManager::fromStdString(n);
            savedGames.push_back(std::move(s));
        }
        elem = elem->NextSiblingElement();
    }
}

void loadFromCommandLine(int n, char *params[])
{
    for (int i = 1 ; i < n ; ++i)
    {
        if (strcmp(params[i], "-v") == 0 && i+1 < n)
        {
            tools::debug::addDebugCategory("all");
        }
        else if (strncmp(params[i], "--verbose=", 10) == 0)
        {
            string s(params[i]);
            string t(s.substr(10));
            for (auto c = t.find_last_of(',') ; c != string::npos ; c = t.find_last_of(','))
            {
                tools::debug::addDebugCategory(t.substr(c+1));
                t = t.substr(0, c);
            }
            tools::debug::addDebugCategory(t);
        }
        else
        {
            string s(params[i]);
            if (auto x = s.find('=') ; x != string::npos)
            {
                string option = s.substr(0, x);
                string value = s.substr(x+1);

                const auto& op = _options.find(tools::hash(option.c_str()));
                if (op != _options.end())
                {
                    if (op->second.is<bool>())
                    {
                        bool b = (value == "0") ? false : true;
                        addOption<bool>(option, b);
                    }
                    else if (op->second.is<unsigned>())
                    {
                        unsigned u = textManager::toInt(value);
                        addOption<unsigned>(option, u);
                    }
                    else if (op->second.is<string>())
                    {
                        addOption<string>(option, value);
                    }
                }
                else
                {
                    tools::debug::warning("Unknown command line option: " + option, "devilsai", __FILENAME__, __LINE__);
                }
            }
            else
            {
                tools::debug::warning("Ill-formed command line option: " + s, "devilsai", __FILENAME__, __LINE__);
            }
        }
    }
}

void Save_Options()
{
    XMLDocument file;

    XMLElement* elem = file.NewElement("options");
    file.InsertFirstChild(elem);

    for (auto& o : _options)
    {
        XMLElement* option = file.NewElement("option");
        option->SetAttribute("hash", o.first);
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
        elem->InsertEndChild(option);
    }

    for (auto& s : savedGames)
    {
        XMLElement* savedGame = file.NewElement("savedGame");
        savedGame->SetAttribute("directory", s.directory.c_str());
        savedGame->SetAttribute("version", s.version.c_str());
        savedGame->SetAttribute("playerName", s.playerName.toStdString().c_str());
        elem->InsertEndChild(savedGame);
    }

    file.SaveFile((tools::filesystem::getSaveDirectoryPath() + "options.xml").c_str());
}

void changeOption(const string& name, const string& value)
{
    if (name == "option-change-language")
    {
        addOption<string>(tools::hash("language"), value);
    }
    else if (name == "option-change-resolution")
    {
        size_t x = value.find("x");
        unsigned newWidth = textManager::toInt(value);
        unsigned newHeight = textManager::toInt(value.substr(x+1));

        if (newWidth != option<unsigned>(tools::hash("screen-width")) || newHeight != option<unsigned>(tools::hash("screen-height")))
        {
            addOption<unsigned>(tools::hash("screen-width"), newWidth);
            addOption<unsigned>(tools::hash("screen-height"), newHeight);
            createWindow();
        }
    }
    else if (name == "option-change-fullscreen")
    {
        bool newMode = (value == "enabled" ? true : false);

        if (newMode != option<bool>(tools::hash("screen-fullscreen")))
        {
            addOption<bool>(tools::hash("screen-fullscreen"), newMode);
            createWindow();
        }
    }
    else if (name == "option-change-console")
    {
        addOption<bool>(tools::hash("show-console"), (value == "enabled" ? true : false));
    }
}

void createNewSavedGamePack()
{
    SavedGame s;

    s.directory = textManager::toString(option<unsigned>(tools::hash("next-game-number")), 4) + "/";
    s.playerName = gamedata::player()->displayedName();
    s.version = "master";

    tools::filesystem::createDirectory(tools::filesystem::getSaveDirectoryPath() + s.directory);

    savedGames.push_back(std::move(s));
    addOption<unsigned>(tools::hash("next-game-number"), option<unsigned>(tools::hash("next-game-number")) + 1);

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

void deleteSavedGamePack(const string& directory)
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
            string filename = elem->Attribute("path");
            tools::filesystem::removeFile(path + filename);
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
    gui::ScrollingList* scrollingList = dynamic_cast<gui::ScrollingList*>(window.widget("load-game-savedgames-list"));

    scrollingList->removeAllEntries();

    for (auto& s : savedGames)
        scrollingList->addEntry(s.playerName, s.directory);

    if (!savedGames.empty())
        tools::signals::addSignal("main-menu:enable-load-game");
    else
        tools::signals::addSignal("main-menu:disable-load-game");
}

void initOptionsWindow(gui::Window& window)
{
    string d;
    gui::optionType o;

    d = option<string>(tools::hash("language"));
    o.set<string>(d);
    window.setValue("options-chooser-language", o);

    d = textManager::toString(option<unsigned>(tools::hash("screen-width"))) + "x" + textManager::toString(option<unsigned>(tools::hash("screen-height")));
    o.set<string>(d);
    window.setValue("options-chooser-resolution", o);

    d = (option<bool>(tools::hash("screen-fullscreen")) ? "enabled" : "disabled");
    o.set<string>(d);
    window.setValue("options-chooser-fullscreen", o);

    d = (option<bool>(tools::hash("show-console")) ? "enabled" : "disabled");
    o.set<string>(d);
    window.setValue("options-chooser-console", o);
}

} //namespace options
