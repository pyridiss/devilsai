
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
#include <iostream>

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
Classe_Jeu::Classe_Options Options;


/** *** **/

void GestionArguments(int n, char *params[])
{
    //Default options
    options::addOption<bool>(tools::math::sdbm_hash("displayShapes"), false);
    options::addOption<bool>(tools::math::sdbm_hash("verbose"), false);
    options::addOption<bool>(tools::math::sdbm_hash("verbose-files"), false);
    options::addOption<bool>(tools::math::sdbm_hash("verbose-images"), false);
    options::addOption<bool>(tools::math::sdbm_hash("verbose-lists"), false);
    options::addOption<bool>(tools::math::sdbm_hash("verbose-lua"), false);
    options::addOption<bool>(tools::math::sdbm_hash("verbose-musics"), false);

	for (int i = 0 ; i < n ; ++i)
	{
		#ifdef DEBOGAGE
		if (strcmp(params[i], "-v") == 0 || strcmp(params[i], "--verbose") == 0)
		{
            options::addOption<bool>(tools::math::sdbm_hash("verbose"), true);
		}
        if (strcmp(params[i], "fichiers") == 0 && options::option<bool>(tools::math::sdbm_hash("verbose")))
		{
			cout << "Mode VERBOSE : FICHIERS activé" << endl;
            options::addOption<bool>(tools::math::sdbm_hash("verbose-files"), true);
            tools::debug::addDebugCategory("files");
		}
        if (strcmp(params[i], "images") == 0 && options::option<bool>(tools::math::sdbm_hash("verbose")))
		{
			cout << "Mode VERBOSE : IMAGES activé" << endl;
            options::addOption<bool>(tools::math::sdbm_hash("verbose-images"), true);
            tools::debug::addDebugCategory("images");
		}
        if (strcmp(params[i], "listes") == 0 && options::option<bool>(tools::math::sdbm_hash("verbose")))
		{
			cout << "Mode VERBOSE : LISTES CHAINEES activé" << endl;
            options::addOption<bool>(tools::math::sdbm_hash("verbose-lists"), true);
            tools::debug::addDebugCategory("lists");
		}
        if (strcmp(params[i], "lua") == 0 && options::option<bool>(tools::math::sdbm_hash("verbose")))
		{
			cout << "Mode VERBOSE : LUA activé" << endl;
            options::addOption<bool>(tools::math::sdbm_hash("verbose-lua"), false);
            tools::debug::addDebugCategory("lua");
		}
        if (strcmp(params[i], "music") == 0 && options::option<bool>(tools::math::sdbm_hash("verbose")))
		{
			cout << "Mode VERBOSE : MUSIC activé" << endl;
            options::addOption<bool>(tools::math::sdbm_hash("verbose-musics"), false);
            tools::debug::addDebugCategory("musics");
		}
		if (strcmp(params[i], "-m") == 0)
		{
            options::addOption<bool>(tools::math::sdbm_hash("displayShapes"), true);
		}
		#endif
	}
}

void createWindow()
{
    unsigned w = options::option<unsigned>(tools::math::sdbm_hash("screen-width"));
    unsigned h = options::option<unsigned>(tools::math::sdbm_hash("screen-height"));

    VideoMode vm = VideoMode(w, h, VideoMode::getDesktopMode().bitsPerPixel);

    if (options::option<bool>(tools::math::sdbm_hash("screen-fullscreen")) && vm.isValid())
    {
        Jeu.App.create(vm, "devilsai", Style::Fullscreen);
    }
    else
    {
        Jeu.App.create(vm, "devilsai");
    }

    Jeu.App.setFramerateLimit(60);
    Jeu.App.setKeyRepeatEnabled(false);
    Jeu.App.setVerticalSyncEnabled(true);

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

	PHYSFS_init(params[0]);

    options::Load_Options();
    tools::timeManager::setSpeed(60.0/1000.0);

    GestionArguments(n, params);

    createWindow();

	srand(time(NULL));

    gui::style::initStyle();

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
