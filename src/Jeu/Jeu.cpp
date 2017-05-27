
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

#include "tools/filesystem.h"
#include "tools/timeManager.h"
#include "tools/signals.h"
#include "gui/window.h"
#include "musicManager/musicManager.h"

#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"


void Load_Chapitre(int Id)
{
	string fileName = INSTALL_DIR + "chapitre/" + intToString(Id) + ".chp";

	ifstream fileStream(fileName.c_str(), ios_base::in);

	if (!fileStream.good()) Erreur("Le fichier suivant n'a pu être chargé :", fileName.c_str());
	if (fileStream.good()) MESSAGE(" Fichier \"" + fileName + "\" ouvert", FICHIER)

	string TypeDonnee = "", bufferString = "";

	while (fileStream.rdstate() == 0)
	{
		fileStream >> TypeDonnee >> bufferString;

		if		(TypeDonnee == "CARTE")	Load_Carte(bufferString, TYPE_CARTE);
		else if	(TypeDonnee == "LISTE")	Load_Carte(bufferString, TYPE_LISTE);
		else if (TypeDonnee == "QUEST")	addQuest(bufferString, "true");

		TypeDonnee = "";
	}

	fileStream.close();
}

/** ÉTABLISSEMENT D'UN NOUVEAU JEU OU CHARGEMENT DU JEU ENREGISTRÉ */

bool RechercheJoueur()
{
	auto carte = Partie.maps.begin();
	bool PersonnageTrouve = false;
	list<Element_Carte*>::iterator tmpj;

	while (carte != Partie.maps.end() && !PersonnageTrouve)
	{
		//Recherche du personnage qui contrôlera le jeu
		tmpj = carte->second.elements.begin();
		while(tmpj != carte->second.elements.end() && !PersonnageTrouve)
		{
			if ((*tmpj)->Get_Controle() == HUMAIN)
			{
				PersonnageTrouve = true;
				Partie.CarteCourante = &(carte->second); //CarteCourante est initialisée à la carte contenant le personnage
			}
			else ++tmpj;
		}

		++carte;
	}

	if (!PersonnageTrouve)
	{
		Erreur("Le jeu demandé ne contient pas de personnage" , "");
		return false;
	}

	Partie.perso = dynamic_cast<Joueur*>((*tmpj));

	if (Partie.perso == NULL)
	{
		Erreur("Le personnage trouvé n'est pas conforme" , "");
		return false;
	}

	Set_PosCarte(Partie.perso->PosX, Partie.perso->PosY, false);
	Set_PosCarte(0, 0, true);

	//Mise en place des éléments dans la liste de collisions :
	for (auto& tmp : Partie.CarteCourante->elements)
	{
		if (tmp->Get_Controle() != HUMAIN) tmp->Set_Controle(AI);
	}

	return true;
}

bool PartieSauvegardee()
{
	Partie.SAVE = ChoixSauvegarde();

	if (Partie.SAVE == "ANNULER")
	{
		Partie.SAVE = "";
        tools::signals::addSignal("main-menu");
		return false;
	}

    Load_Partie(Partie.SAVE);

	MESSAGE(">> Chargement de la sauvegarde terminée <<", FICHIER)

	bool joueur = RechercheJoueur();
	if (!joueur) return false;

    musicManager::playMusic(Partie.CarteCourante->ambience);

	MESSAGE(">> Mise en place du jeu sauvegardé terminée <<", FICHIER)

	return true;
}

/** FONCTIONS DE GESTION DE LA PARTIE **/

void mainLoop()
{
	float ChangementLieu = 255;
	float SauvegardeEffectuee = 255;

    bool managementActivated = false;
    bool playerResting = false;

	String32 NomLieu;

    gui::Window mainMenuWindow("gui/main-menu.xml", Jeu.App);
    gui::Window newGameWindow("gui/new-game.xml", Jeu.App);
    gui::Window confirmExitGameWindow("gui/confirm-exit-game.xml", Jeu.App);
    gui::Window ingameMenuWindow("gui/ingame-menu.xml", Jeu.App);
    gui::Window ingameToolbar("gui/ingame-toolbar.xml", Jeu.App);
    gui::Window loadingWindow("gui/loading.xml", Jeu.App);
    gui::Window playerDeadWindow("gui/player-dead.xml", Jeu.App);

    ingameToolbar.startWindow(Jeu.App);
    loadingWindow.startWindow(Jeu.App);

    tools::signals::addSignal("main-menu");

	while (true)
	{
        //1. Events & Signals

        Event event;
        while (Jeu.App.pollEvent(event))
        {
            ingameToolbar.manage(Jeu.App, event);

            if (Partie.currentUserScreen != nullptr && Partie.currentUserScreen->manageFunction != nullptr)
                Partie.currentUserScreen->manageFunction(event);

            if (event.type == Event::Closed || (Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
                return;
        }

        tools::signals::Signal signal = tools::signals::getNextSignal();
        while (signal.first != "")
        {
            if (signal.first == "main-menu")
            {
                mainMenuWindow.manage(Jeu.App);
                managementActivated = false;
            }

            if (signal.first == "savegame")
            {
                Partie.currentUserScreen = nullptr;
                Disp_Menu();
                Disp_JaugesVie();
                Disp_Consoles();
                SauvegardeEffectuee = 254;
                Save_Partie();
            }

            if (signal.first == "rest")
            {
                managementActivated = false;
                playerResting = true;
                Partie.perso->Repos();
            }

            if (signal.first == "player-dead")
            {
                playerDeadWindow.manage(Jeu.App);
                Clean_Partie();
            }

            if (signal.first == "new-game")
            {
                newGameWindow.manage(Jeu.App);
            }

            if (signal.first == "start-game-chapter-1")
            {
                loadingWindow.display(Jeu.App);
                Jeu.App.display();
                Load_Chapitre(1);
                RechercheJoueur();
                Partie.perso->Nom = signal.second.String32Data;
                musicManager::playMusic(Partie.CarteCourante->ambience);
                managementActivated = true;
            }

            if (signal.first == "start-game-tutorial")
            {
                loadingWindow.display(Jeu.App);
                Jeu.App.display();
                Load_Chapitre(0);
                RechercheJoueur();
                Partie.perso->Nom = signal.second.String32Data;
                musicManager::playMusic(Partie.CarteCourante->ambience);
                managementActivated = true;
            }

            if (signal.first == "load-game")
            {
                if (PartieSauvegardee())
                {
                    managementActivated = true;
                }
            }

            if (signal.first == "options")
            {
                EcranOptions();
            }

            if (signal.first == "ask-menu")
            {
                ingameMenuWindow.manage(Jeu.App);
            }

            if (signal.first == "screen-character")
            {
                Partie.changeCurrentUserScreen(&Partie.screenCharacter);
            }

            if (signal.first == "screen-equipment")
            {
                Partie.changeCurrentUserScreen(&Partie.screenEquipment);
            }

            if (signal.first == "screen-skills")
            {
                Partie.changeCurrentUserScreen(&Partie.screenSkills);
            }

            if (signal.first == "screen-journal")
            {
                Partie.changeCurrentUserScreen(&Partie.screenJournal);
                tools::signals::addSignal("ingame-toolbar:remove-warn-journal");
            }

            if (signal.first == "ask-exit")
            {
                confirmExitGameWindow.manage(Jeu.App);
            }

            if (signal.first == "exit")
            {
                return;
            }

            tools::signals::removeSignal();
            signal = tools::signals::getNextSignal();
        }

        if (Partie.CarteCourante == NULL)
        {
            managementActivated = false;
            continue;
        }

        //2. Management

        if (managementActivated)
            Partie.CarteCourante->GestionElements();

        //3. Display

        Jeu.App.clear();
        Partie.CarteCourante->displayBackground();
        Partie.CarteCourante->display();

        if (!Partie.ModeCinematiques)
        {
            Disp_Menu();
            Disp_JaugesVie();
            Disp_Consoles();
        }

        ingameToolbar.display(Jeu.App);

        if (Partie.currentUserScreen != NULL)
            Partie.currentUserScreen->dispFunction();
        else
            Disp_MiniaturesCompetences();

        Disp_FonduNoir(0);

        if (playerResting)
        {
            bool animationFinished = Disp_Repos();
            if (animationFinished)
            {
                playerResting = false;
                managementActivated = true;
            }
        }

        Disp_FPS();

		//2. GESTION DES MISSIONS

		manageQuests();

		//4. CHANGEMENTS DE LIEU

		if (Partie.perso->IndiceLieu != Partie.perso->SauvegardeIndiceLieu)
		{
            if (Partie.perso->LieuVillage != "village")
                tools::signals::addSignal("ingame-toolbar:disable-rest");
            else
                tools::signals::addSignal("ingame-toolbar:enable-rest");

			ChangementLieu = 254;
			NomLieu = getTranslatedNameOfPlace(Partie.perso->IndiceLieu);
			Partie.perso->SauvegardeIndiceLieu = Partie.perso->IndiceLieu;
		}
		if (ChangementLieu != 255)
		{
			ChangementLieu -= tools::timeManager::I((256-ChangementLieu)/24);
			if (ChangementLieu <= 0) ChangementLieu = 255;
			else
			{
				Disp_TexteCentre(NomLieu, Options.ScreenW/2 + 1, 121, Color(0, 0, 0, ChangementLieu), 30., Jeu.DayRoman);
				Disp_TexteCentre(NomLieu, Options.ScreenW/2, 120, Color(225, 0, 0, ChangementLieu), 30., Jeu.DayRoman);

			}
		}

		//5. SAUVEGARDE EFFECTUÉE

		if (SauvegardeEffectuee != 255)
		{
			SauvegardeEffectuee -= tools::timeManager::I((256-SauvegardeEffectuee)/24);
			if (SauvegardeEffectuee <= 0) SauvegardeEffectuee = 255;
			else
			{
				Disp_TexteCentre(_SAUVEGARDE, Options.ScreenW/2 + 1, 161, Color(0, 0, 0, SauvegardeEffectuee), 30., Jeu.DayRoman);
				Disp_TexteCentre(_SAUVEGARDE, Options.ScreenW/2, 160, Color(225, 0, 0, SauvegardeEffectuee), 30., Jeu.DayRoman);

			}
		}

		//7. CHARGEMENT DU JEU APRÈS LE DIDACTICIEL

		if (Partie.loadFirstChapter)
		{
			String32 NomPersonnage = Partie.perso->Nom;
			Clean_Partie();
			Load_Chapitre(1);
			bool joueur = RechercheJoueur();
			if (!joueur)
			{
				Erreur("Le passage au chapitre 1 a échoué", "");
				return;
			}
			Partie.perso->Nom = NomPersonnage;
			Partie.loadFirstChapter = false;
		}

        tools::timeManager::frameDone();
		Jeu.App.display();
        musicManager::manageRunningMusics();
	}
}

void Clean_Partie()
{
	Partie.perso = NULL;
	Partie.CarteCourante = NULL;
	Partie.CoffreOuvert = NULL;
	Partie.selectedObject = nullptr;
	Partie.selectedSkill = nullptr;
	Partie.listDialogs.clear();
	Partie.currentUserScreen = nullptr;
	Partie.journal.entries.clear();

	Set_PosCarte(0, 0, false);
	Set_PosCarte(0, 0, true);

	deleteQuests();

	SupprimerLignesConsoles();

	SupprimerListe_Carte();

	deleteCommonClasses();
	deleteLandsClasses();
	deleteMovingLandsClasses();
}
