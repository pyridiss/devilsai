
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

#include <physfs.h>

#include <string.h>

#include "gui/style.h"
#include "tools/timeManager.h"
#include "musicManager/musicManager.h"

#include "../Bibliotheque/Constantes.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "../Carte/Carte.h"

#include "Jeu.h"

#include "imageManager/imageManager.h"

/** VARIABLES GLOBALES **/

//Création du jeu et de la partie :
Classe_Jeu Jeu;
Classe_Partie Partie;
Classe_Jeu::Classe_Arguments Arguments;
Classe_Jeu::Classe_Options Options;


/** CONSTRUCTEURS ET DESTRUCTEURS DES CLASSES Jeu ET Partie **/

Classe_Jeu::Classe_Jeu()
{
	Ind_AI = NULL;
}

Classe_Jeu::~Classe_Jeu()
{
	if (Ind_AI != NULL) delete Ind_AI;
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

void Classe_Partie::changeCurrentUserScreen(UserScreen* _new)
{
	if (_new == nullptr) currentUserScreen = nullptr;
	else
	{
		if (currentUserScreen != _new)
			currentUserScreen = _new;
		else currentUserScreen = nullptr;
	}
}

/** *** **/

void GestionArguments(int n, char *params[])
{
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
		}
		if (strcmp(params[i], "images") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : IMAGES activé" << endl;
			Arguments.TabVerbose[IMAGE] = true;
		}
		if (strcmp(params[i], "listes") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : LISTES CHAINEES activé" << endl;
			Arguments.TabVerbose[LISTE] = true;
		}
		if (strcmp(params[i], "lua") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : LUA activé" << endl;
			Arguments.TabVerbose[LUA] = true;
		}
		if (strcmp(params[i], "music") == 0 && Arguments.TabVerbose[0])
		{
			cout << "Mode VERBOSE : MUSIC activé" << endl;
			Arguments.TabVerbose[MUSIC] = true;
		}
		if (strcmp(params[i], "-m") == 0 || strcmp(params[i], "--masks") == 0)
		{
			cout << "Mode MASKS activé" << endl;
			Arguments.Masks = true;
		}
		if (strcmp(params[i], "--no-save") == 0)
		{
			cout << "Mode SAVE DISABLED activé" << endl;
			Arguments.SaveDisabled = true;
		}
		#endif
		if (strcmp(params[i], "--no-limit-fps") == 0)
		{
			cout << "Mode LIMIT TO FPS DISABLED activé" << endl;
			Arguments.LimitToFpsDisabled = true;
		}
	}
}


/** DÉBUT DU PROGRAMME **/
int main(int n, char *params[])
{
	#ifdef DEBOGAGE
	cout << "Compilé avec DEBOGAGE activé" << endl;
	#endif

	GestionArguments(n, params);

	PHYSFS_init(params[0]);

	Load_Options();
    tools::timeManager::setSpeed(Options.VitesseJeu);

	if (!Options.PleinEcran) Jeu.App.create(VideoMode(Options.ScreenW, Options.ScreenH, VideoMode::getDesktopMode().bitsPerPixel), "Devilsai");
	else
	{
		VideoMode vm = VideoMode(Options.ScreenW, Options.ScreenH, VideoMode::getDesktopMode().bitsPerPixel);
		if (vm.isValid()) Jeu.App.create(vm, "Devilsai", Style::Fullscreen);
		else Jeu.App.create(vm, "Devilsai");
	}

	if (!Arguments.LimitToFpsDisabled) Jeu.App.setFramerateLimit(60);
	Jeu.App.setKeyRepeatEnabled(false);
	Jeu.App.setVerticalSyncEnabled(true);

	srand(time(NULL));

	loadAvailableLanguages();
	loadDevilsaiMessages();
	MESSAGE("Fichiers de langue chargés", FICHIER)

    gui::style::initStyle();

	Jeu.Cardinal.loadFromFile(INSTALL_DIR + "Cardinal.ttf");
	Jeu.DayRoman.loadFromFile(INSTALL_DIR + "DayRoman.ttf");
	Load_Decorations();
	MESSAGE("Décorations chargées", FICHIER)

	LectureSauvegardes();
	MESSAGE("Sauvegardes chargées", FICHIER)


	Jeu.Ind_AI = new Individu_Unique();
	Jeu.Ind_AI->collisionType = RectangleCollision;
	Jeu.Ind_AI->Id = -1;

    imageManager::addContainer("individuals");
    imageManager::addContainer("movingObjects");

    musicManager::playMusic("Gates_Of_Heaven");

	short choice = 0;
	while (choice != QUITTER)
	{
		choice = MenuPrincipal();

		switch (choice)
		{
			case NOUVEAU :	if (NouvellePartie())
							{
                                musicManager::stopMusic("Gates_Of_Heaven");
								EcranJeu(true);
								Clean_Partie();
                                musicManager::playMusic("Gates_Of_Heaven");
							}
							break;
			case CHARGER :	if (PartieSauvegardee())
							{
                                musicManager::stopMusic("Gates_Of_Heaven");
								EcranJeu(false);
								Clean_Partie();
                                musicManager::playMusic("Gates_Of_Heaven");
							}
							break;
			case OPTIONS :	EcranOptions();
			case QUITTER :	break;
		}
	}

	Supprimer_Decorations();
	deleteLanguagesList();

	if (Arguments.LimitToFpsDisabled) MoyenneFPS();

	PHYSFS_deinit();

	Save_Options();
	Save_Sauvegardes();

	return EXIT_SUCCESS;
}
