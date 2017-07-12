
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

#include <tinyxml2.h>

#include "tools/signals.h"
#include "tools/filesystem.h"
#include "tools/textManager.h"

#include "../Bibliotheque/Bibliotheque.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"

#include "gui/window.h"

#include "gamedata.h"

using namespace tinyxml2;

struct SavedGame
{
    string directory;
    String32 playerName;
    string version;
};

list<SavedGame> savedGames;
SavedGame* currentSavedGame = nullptr;
int nextGameNumber = 1;

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
            elem->QueryAttribute("resolution-x", &Options.ScreenW);
            elem->QueryAttribute("resolution-y", &Options.ScreenH);
            elem->QueryAttribute("fullscreen", &Options.PleinEcran);
            elem->QueryAttribute("console", &Options.AffichageDegats);
            elem->QueryAttribute("next-game-number", &nextGameNumber);
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

	Options.PleinEcran_Save = Options.PleinEcran;
	Options.ScreenW_Save = Options.ScreenW;
	Options.ScreenH_Save = Options.ScreenH;
}

void Save_Options()
{
    XMLDocument file;

    XMLElement* elem = file.NewElement("options");
    file.InsertFirstChild(elem);

    XMLElement* config = file.NewElement("configuration");
    config->SetAttribute("language", Options.Langue.c_str());
    config->SetAttribute("resolution-x", Options.ScreenW_Save);
    config->SetAttribute("resolution-y", Options.ScreenH_Save);
    config->SetAttribute("fullscreen", Options.PleinEcran_Save);
    config->SetAttribute("console", Options.AffichageDegats);
    config->SetAttribute("next-game-number", nextGameNumber);
    elem->InsertEndChild(config);

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
        Options.ScreenW_Save = stoi(value);
        Options.ScreenH_Save = stoi(value.substr(x+1));
    }
    else if (option == "option-change-fullscreen")
    {
        Options.PleinEcran_Save = (value == "enabled" ? true : false);
    }
    else if (option == "option-change-console")
    {
        Options.AffichageDegats = (value == "enabled" ? true : false);
    }
}

void createNewSavedGamePack()
{
    SavedGame s;

    s.directory = intToString(nextGameNumber, 4);
    s.playerName = gamedata::player()->Nom;
    s.version = "master";

    tools::filesystem::createDirectory(tools::filesystem::getSaveDirectoryPath() + s.directory);

    savedGames.push_back(std::move(s));
    ++nextGameNumber;

    currentSavedGame = &savedGames.back();
    Save_Options();
}

void updateCurrentSavedGamePack()
{
    if (currentSavedGame == nullptr)
        createNewSavedGamePack();

    currentSavedGame->version = "master";
}

void initOptionsWindow(gui::Window& window)
{
    tools::signals::SignalData d;

    if (Options.Langue == "fr=") d.stringData = "FR";
    else d.stringData = "EN";
    window.setValue("chooser-language", d);

    d.stringData = intToString(Options.ScreenW_Save) + "x" + intToString(Options.ScreenH_Save);
    window.setValue("chooser-resolution", d);

    d.stringData = (Options.PleinEcran ? "enabled" : "disabled");
    window.setValue("chooser-fullscreen", d);

    d.stringData = (Options.AffichageDegats ? "enabled" : "disabled");
    window.setValue("chooser-console", d);

    if (!savedGames.empty())
        tools::signals::addSignal("main-menu:enable-load-game");
}
