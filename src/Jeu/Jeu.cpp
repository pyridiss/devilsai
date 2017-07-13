
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

#include "tools/debug.h"
#include "tools/filesystem.h"
#include "tools/timeManager.h"
#include "tools/signals.h"
#include "tools/textManager.h"

#include "gui/window.h"
#include "gui/textWidget.h"

#include "musicManager/musicManager.h"

#include "../Carte/Carte.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"

#include "gamedata.h"
#include "options.h"


class UserScreen
{
	public:
		void (*dispFunction)(void);
		void (*manageFunction)(Event&);
};

UserScreen screenCharacter;
UserScreen screenEquipment;
UserScreen screenSkills;
UserScreen screenJournal;
UserScreen *currentUserScreen = nullptr;

void changeCurrentUserScreen(UserScreen* _new)
{
	if (_new == nullptr) currentUserScreen = nullptr;
	else
	{
		if (currentUserScreen != _new)
			currentUserScreen = _new;
		else currentUserScreen = nullptr;
	}
}

void Load_Chapitre(string filename)
{
    ifstream fileStream(tools::filesystem::dataDirectory() + filename, ios_base::in);

    if (fileStream.good()) tools::debug::message("File open: " + filename, "files");
    else tools::debug::error("File note found: " + filename, "files");

	string TypeDonnee = "", bufferString = "";

	while (fileStream.rdstate() == 0)
	{
		fileStream >> TypeDonnee >> bufferString;

		if		(TypeDonnee == "CARTE")	Load_Carte(bufferString, TYPE_CARTE);
		else if	(TypeDonnee == "LISTE")	Load_Carte(bufferString, TYPE_LISTE);
        else if (TypeDonnee == "loadXMLFile")
        {
            string b2;
            fileStream >> b2;
            gamedata::world("1")->loadFromFile(tools::filesystem::dataDirectory() + bufferString, b2);
        }
		else if (TypeDonnee == "QUEST")	addQuest(bufferString, "true");

		TypeDonnee = "";
	}

	fileStream.close();
}


/** FONCTIONS DE GESTION DE LA PARTIE **/

void mainLoop()
{
    screenCharacter.dispFunction = Disp_Personnage;
    screenCharacter.manageFunction = nullptr;

    screenEquipment.dispFunction = Disp_Equipement;
    screenEquipment.manageFunction = Gestion_Coffre;

    screenSkills.dispFunction = Disp_Competences;
    screenSkills.manageFunction = Gestion_Competences;

    screenJournal.dispFunction = displayJournal;
    screenJournal.manageFunction = nullptr;

	float ChangementLieu = 255;
	float SauvegardeEffectuee = 255;

    bool managementActivated = false;
    bool playerResting = false;

    gui::Window mainMenuWindow("gui/main-menu.xml", Jeu.App);
    gui::Window newGameWindow("gui/new-game.xml", Jeu.App);
    gui::Window loadGameWindow("gui/load-game.xml", Jeu.App);
    gui::Window optionsWindow("gui/options.xml", Jeu.App);
    gui::Window confirmExitGameWindow("gui/confirm-exit-game.xml", Jeu.App);
    gui::Window ingameMenuWindow("gui/ingame-menu.xml", Jeu.App);
    gui::Window ingameToolbar("gui/ingame-toolbar.xml", Jeu.App);
    gui::Window loadingWindow("gui/loading.xml", Jeu.App);
    gui::Window playerDeadWindow("gui/player-dead.xml", Jeu.App);

    gui::TextWidget placeName;
    placeName.setCenterCoordinates(Options.ScreenW / 2, 120);
    placeName.setTextFont(gui::style::fontFromString("dayroman"), 30);
    placeName.setTextOutline(Color(0, 0, 0), 1);
    placeName.setTextColor("normal", Color(192, 0, 0));

    ingameToolbar.startWindow(Jeu.App);
    loadingWindow.startWindow(Jeu.App);

    options::initLoadGameWindow(loadGameWindow);
    options::initOptionsWindow(optionsWindow);

    tools::signals::addSignal("main-menu");
    tools::signals::addSignal("main-menu:disable-load-game");

    View worldView(FloatRect(0, 0, Options.ScreenW, Options.ScreenH));
    worldView.setViewport(sf::FloatRect(0, 0, 1, 1));

    Individu_Unique cursor;
    cursor.size.circle(tools::math::Vector2d(0, 0), 5);
    cursor.interactionField.profile = tools::math::Shape::Profiles::None;
    cursor.viewField.profile = tools::math::Shape::Profiles::None;
    Individu* underCursor = NULL;
    bool cursorIsInWorld = false;

	while (true)
	{
        //1. Events & Signals

        Event event;
        while (Jeu.App.pollEvent(event))
        {
            ingameToolbar.manage(Jeu.App, event);

            if (currentUserScreen != nullptr && currentUserScreen->manageFunction != nullptr)
                currentUserScreen->manageFunction(event);

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
                SauvegardeEffectuee = 254;
                Save_Partie();
            }

            if (signal.first == "rest")
            {
                managementActivated = false;
                playerResting = true;
                gamedata::player()->Repos();
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
                Load_Chapitre("chapitre/1.chp");
                gamedata::player()->Nom = signal.second.String32Data;
                musicManager::playMusic(gamedata::currentWorld()->ambience);
                managementActivated = true;
            }

            if (signal.first == "start-game-tutorial")
            {
                loadingWindow.display(Jeu.App);
                Jeu.App.display();
                Load_Chapitre("chapitre/0.chp");
                gamedata::player()->Nom = signal.second.String32Data;
                musicManager::playMusic(gamedata::currentWorld()->ambience);
                managementActivated = true;
            }

            if (signal.first == "load-game")
            {
                loadGameWindow.manage(Jeu.App);
            }

            if (signal.first == "delete-game")
            {
                options::deleteSavedGamePack(signal.second.stringData);
                options::initLoadGameWindow(loadGameWindow);
            }

            if (signal.first == "start-loaded-game")
            {
                Load_Partie(signal.second.stringData);
                managementActivated = true;
                musicManager::playMusic(gamedata::currentWorld()->ambience);
            }

            if (signal.first == "options")
            {
                optionsWindow.manage(Jeu.App);
            }

            if (signal.first.find("option-change") != string::npos)
            {
                options::changeOption(signal.first, signal.second.stringData);
            }

            if (signal.first == "ask-menu")
            {
                ingameMenuWindow.manage(Jeu.App);
            }

            if (signal.first == "screen-character")
            {
                changeCurrentUserScreen(&screenCharacter);
            }

            if (signal.first == "screen-equipment")
            {
                changeCurrentUserScreen(&screenEquipment);
            }

            if (signal.first == "screen-skills")
            {
                changeCurrentUserScreen(&screenSkills);
            }

            if (signal.first == "screen-journal")
            {
                changeCurrentUserScreen(&screenJournal);
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

            if (signal.first == "place-changed")
            {
                ChangementLieu = 254;
                placeName.setAllText(tools::textManager::getText("places", signal.second.stringData));
                placeName.updateTextPosition();
            }

            tools::signals::removeSignal();
            signal = tools::signals::getNextSignal();
        }

        if (gamedata::currentWorld() == NULL)
        {
            managementActivated = false;
            continue;
        }

        //2. Management

        if (managementActivated)
            gamedata::currentWorld()->GestionElements(worldView);

        worldView.setCenter(gamedata::player()->position().x, gamedata::player()->position().y);

        //Mouse cursor
        cursor.move(-cursor.position().x + Jeu.App.mapPixelToCoords(Mouse::getPosition(Jeu.App), worldView).x,
                    -cursor.position().y + Jeu.App.mapPixelToCoords(Mouse::getPosition(Jeu.App), worldView).y);

        if (Mouse::getPosition(Jeu.App).x >= Jeu.App.getSize().x * worldView.getViewport().left &&
            Mouse::getPosition(Jeu.App).y >= Jeu.App.getSize().y * worldView.getViewport().top &&
            Mouse::getPosition(Jeu.App).x <= Jeu.App.getSize().x * (worldView.getViewport().left + worldView.getViewport().width) &&
            Mouse::getPosition(Jeu.App).y <= Jeu.App.getSize().y * (worldView.getViewport().top + worldView.getViewport().height))
            cursorIsInWorld = true;
        else cursorIsInWorld = false;

        gamedata::currentWorld()->resetCollisionManager();
        underCursor = NULL;
        int Resultat = COLL_OK;

        while(cursorIsInWorld && Resultat == COLL_OK)
        {
            Resultat = gamedata::currentWorld()->browseCollisionList(&cursor);

            if (Resultat == COLL_PRIM_MVT)
            {
                underCursor = dynamic_cast<Individu*>(gamedata::currentWorld()->getCurrentCollider());
                break;
            }
        }

        //Mouse click
        if (cursorIsInWorld && Mouse::isButtonPressed(Mouse::Left))
        {
            gamedata::player()->automove = true;
            gamedata::player()->automoveEndpoint.x = cursor.position().x;
            gamedata::player()->automoveEndpoint.y = cursor.position().y;
        }

        //3. Display

        Jeu.App.clear();
        Jeu.App.setView(worldView);

        gamedata::currentWorld()->displayBackground(Jeu.App);
        gamedata::currentWorld()->display(Jeu.App);

        Jeu.App.setView(Jeu.App.getDefaultView());

        if (!Partie.ModeCinematiques)
        {
            Disp_Menu();
            Disp_JaugesVie();
            if (underCursor != NULL)
                underCursor->displayLifeGauge(Mouse::getPosition(Jeu.App).x, Mouse::getPosition(Jeu.App).y);
            Disp_Consoles();
        }

        ingameToolbar.display(Jeu.App);

        if (currentUserScreen != NULL)
            currentUserScreen->dispFunction();
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

        gamedata::updateCurrentPlace();

		if (ChangementLieu != 255)
		{
			ChangementLieu -= tools::timeManager::I((256-ChangementLieu)/24);
			if (ChangementLieu <= 0) ChangementLieu = 255;
			else
			{
                placeName.display(Jeu.App);
			}
		}

		//5. SAUVEGARDE EFFECTUÉE

		if (SauvegardeEffectuee != 255)
		{
			SauvegardeEffectuee -= tools::timeManager::I((256-SauvegardeEffectuee)/24);
			if (SauvegardeEffectuee <= 0) SauvegardeEffectuee = 255;
			else
			{
				Disp_TexteCentre(tools::textManager::getText("devilsai", "SAUVEGARDE"), Options.ScreenW/2 + 1, 161, Color(0, 0, 0, SauvegardeEffectuee), 30., Jeu.DayRoman);
				Disp_TexteCentre(tools::textManager::getText("devilsai", "SAUVEGARDE"), Options.ScreenW/2, 160, Color(225, 0, 0, SauvegardeEffectuee), 30., Jeu.DayRoman);

			}
		}

		//7. CHARGEMENT DU JEU APRÈS LE DIDACTICIEL

		if (Partie.loadFirstChapter)
		{
			String32 NomPersonnage = gamedata::player()->Nom;
			Clean_Partie();
			Load_Chapitre("chapitre/1.chp");
			gamedata::player()->Nom = NomPersonnage;
			Partie.loadFirstChapter = false;
		}

        tools::timeManager::frameDone();
		Jeu.App.display();
        musicManager::manageRunningMusics();
	}
}

void Clean_Partie()
{
	Partie.CoffreOuvert = NULL;
	Partie.selectedObject = nullptr;
	Partie.selectedSkill = nullptr;
	Partie.listDialogs.clear();
	currentUserScreen = nullptr;
	Partie.journal.entries.clear();

	deleteQuests();

	SupprimerLignesConsoles();

    gamedata::clearWorlds();
    gamedata::clearSpecies();
    gamedata::clearInertItemDesigns();
}
