
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

#include "config.h"

#include <physfs.h>

#include <string.h>

#include "gui/style.h"
#include "tools/debug.h"
#include "tools/timeManager.h"
#include "tools/math.h"
#include "tools/filesystem.h"
#include "tools/textManager.h"

#include "Bibliotheque/Bibliotheque.h"
#include "../Bibliotheque/Constantes.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "../Carte/Carte.h"

#include "Jeu.h"

#include "imageManager/imageManager.h"

#include "options.h"

/** VARIABLES GLOBALES **/

//Création du jeu et de la partie :
Classe_Jeu Jeu;
Classe_Jeu::Classe_Arguments Arguments;
Classe_Jeu::Classe_Options Options;


/** CONSTRUCTEURS ET DESTRUCTEURS DES CLASSES Jeu ET Partie **/

Classe_Jeu::Classe_Jeu()
{
}

Classe_Jeu::~Classe_Jeu()
{
}

Classe_Jeu::Classe_Options::Classe_Options()
{
	string form = "%%";
	string eol = "\\";
	string space = " ";
	Utf8::toUtf32(form.begin(), form.end(), back_inserter(CharForm));
	Utf8::toUtf32(eol.begin(), eol.end(), back_inserter(CharEOL));
	Utf8::toUtf32(space.begin(), space.end(), back_inserter(CharSpace));
}


/** *** **/

void GestionArguments(int n, char *params[])
{
    //Default options
    options::addOption<bool>("displayShapes", false);

	for (int i = 0 ; i < n ; ++i)
	{
		#ifdef DEBOGAGE
		if (strcmp(params[i], "-v") == 0 || strcmp(params[i], "--verbose") == 0)
		{
			Arguments.TabVerbose[0] = true;
		}
		if (strcmp(params[i], "fichiers") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : FICHIERS activé" << endl;
			Arguments.TabVerbose[FICHIER] = true;
            tools::debug::addDebugCategory("files");
		}
		if (strcmp(params[i], "images") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : IMAGES activé" << endl;
			Arguments.TabVerbose[IMAGE] = true;
            tools::debug::addDebugCategory("images");
		}
		if (strcmp(params[i], "listes") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : LISTES CHAINEES activé" << endl;
			Arguments.TabVerbose[LISTE] = true;
            tools::debug::addDebugCategory("lists");
		}
		if (strcmp(params[i], "lua") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : LUA activé" << endl;
			Arguments.TabVerbose[LUA] = true;
            tools::debug::addDebugCategory("lua");
		}
		if (strcmp(params[i], "music") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : MUSIC activé" << endl;
			Arguments.TabVerbose[MUSIC] = true;
            tools::debug::addDebugCategory("musics");
		}
		if (strcmp(params[i], "-m") == 0)
		{
            options::addOption<bool>("displayShapes", true);
		}
		#endif
	}
}


/** DÉBUT DU PROGRAMME **/
int main(int n, char *params[])
{
	#ifdef DEBOGAGE
	cout << "Compilé avec DEBOGAGE activé" << endl;
	#endif

    tools::filesystem::createDirectory(tools::filesystem::getSaveDirectoryPath());

    tools::debug::openDebugFile("devilsai", VERSION);
    tools::debug::addDebugCategory("devilsai");
    tools::filesystem::setDataDirectory(INSTALL_DIR);
    tools::math::initLibrary();
    tools::textManager::initLibrary();

	GestionArguments(n, params);

	PHYSFS_init(params[0]);

    options::Load_Options();
    tools::timeManager::setSpeed(Options.VitesseJeu);

	if (!Options.PleinEcran) Jeu.App.create(VideoMode(Options.ScreenW, Options.ScreenH, VideoMode::getDesktopMode().bitsPerPixel), "Devilsai");
	else
	{
		VideoMode vm = VideoMode(Options.ScreenW, Options.ScreenH, VideoMode::getDesktopMode().bitsPerPixel);
		if (vm.isValid()) Jeu.App.create(vm, "Devilsai", Style::Fullscreen);
		else Jeu.App.create(vm, "Devilsai");
	}

    Jeu.App.setFramerateLimit(60);
	Jeu.App.setKeyRepeatEnabled(false);
	Jeu.App.setVerticalSyncEnabled(true);

	srand(time(NULL));

    gui::style::initStyle();

	Jeu.DayRoman.loadFromFile(tools::filesystem::dataDirectory() + "DayRoman.ttf");
	Load_Decorations();
	MESSAGE("Décorations chargées", FICHIER)

    imageManager::addContainer("individuals");
    imageManager::addContainer("movingObjects");
    imageManager::addContainer("paysage");
    imageManager::addContainer("skills");

    mainLoop();
    Clean_Partie();

	Supprimer_Decorations();

	PHYSFS_deinit();

    options::Save_Options();

    tools::debug::message("Closing devilsai debug file.", "devilsai", __FILENAME__, __LINE__);

	return EXIT_SUCCESS;
}
