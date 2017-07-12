
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
#include "Jeu.h"

#include "gui/window.h"

using namespace tinyxml2;

struct SavedGame
{
    string directory;
    String32 playerName;
    string version;
};

list<SavedGame> savedGames;

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

        if (elemName == "language")
        {
            Options.Langue = elem->Attribute("value");
        }
        else if (elemName == "resolution")
        {
            elem->QueryAttribute("x", &Options.ScreenW);
            elem->QueryAttribute("y", &Options.ScreenH);
        }
        else if (elemName == "fullscreen")
        {
            elem->QueryAttribute("value", &Options.PleinEcran);
        }
        else if (elemName == "console")
        {
            elem->QueryAttribute("value", &Options.AffichageDegats);
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

    XMLElement* language = file.NewElement("language");
    language->SetAttribute("value", Options.Langue.c_str());
    elem->InsertEndChild(language);

    XMLElement* resolution = file.NewElement("resolution");
    resolution->SetAttribute("x", Options.ScreenW_Save);
    resolution->SetAttribute("y", Options.ScreenH_Save);
    elem->InsertEndChild(resolution);

    XMLElement* fullscreen = file.NewElement("fullscreen");
    fullscreen->SetAttribute("value", Options.PleinEcran_Save);
    elem->InsertEndChild(fullscreen);

    XMLElement* console = file.NewElement("console");
    console->SetAttribute("value", Options.AffichageDegats);
    elem->InsertEndChild(console);

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
