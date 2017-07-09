
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

#include "tools/signals.h"
#include "tools/filesystem.h"

#include "../Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "Jeu.h"

#include "gui/window.h"

void Load_Options()
{
	bool ChargerDefaut = false;

	string fichier = tools::filesystem::getSaveDirectoryPath() + "options.opt";

	ifstream Fichier(fichier, ios_base::in);
	if (!Fichier.good())
	{
		ChargerDefaut = true;
        tools::filesystem::createDirectory(tools::filesystem::getSaveDirectoryPath());
	}

	if (Fichier.good()) MESSAGE(" Fichier \"" + fichier + "\" ouvert", FICHIER)

	string Test;
	if (!ChargerDefaut)
	{
		Fichier >> Test;
		if (Test != "FICHIER_OPTIONS")
		{
			ChargerDefaut = true;
			Fichier.close();
		}
	}

	if (ChargerDefaut)
	{
		string fichierDef = tools::filesystem::dataDirectory() + "defaut.opt";
		Fichier.open(fichierDef, ios_base::in);

		if (!Fichier.good()) Erreur("Le fichier suivant n'a pu être chargé :", fichierDef);
		if (Fichier.good()) MESSAGE(" Fichier \"" + fichierDef + "\" ouvert", FICHIER)
	}
	string TypeDonnee = "";

	while (Fichier.rdstate() == 0)
	{
		Fichier >> TypeDonnee;

		if (TypeDonnee == "LANGUE")			Fichier >> Options.Langue;
		if (TypeDonnee == "PLEINECRAN")		Fichier >> Options.PleinEcran;
		if (TypeDonnee == "SAUVEGARDE")		Fichier >> Options.SauvegardeDisponible;
		if (TypeDonnee == "DEGATS")			Fichier >> Options.AffichageDegats;
		if (TypeDonnee == "RESOLUTION")		Fichier >> Options.ScreenW >> Options.ScreenH;

		TypeDonnee = "";
	}

	Options.PleinEcran_Save = Options.PleinEcran;
	Options.ScreenW_Save = Options.ScreenW;
	Options.ScreenH_Save = Options.ScreenH;

	Fichier.close();
}

void Save_Options()
{
	string fileName = tools::filesystem::getSaveDirectoryPath() + "options.opt";

	ofstream fileStream(fileName, ios_base::out);

	if (!fileStream.good()) Erreur("Le fichier suivant n'a pu être ouvert en écriture :", fileName);
	if (fileStream.good()) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	fileStream << "FICHIER_OPTIONS" << endl
			   << "LANGUE " << Options.Langue << endl
			   << "PLEINECRAN " << Options.PleinEcran_Save << endl
			   << "SAUVEGARDE " << Options.SauvegardeDisponible << endl
			   << "DEGATS " << Options.AffichageDegats << endl
			   << "RESOLUTION " << Options.ScreenW_Save << " " << Options.ScreenH_Save << endl;

	fileStream.close();
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
}
