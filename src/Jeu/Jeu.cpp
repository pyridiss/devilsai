
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

#include "inventoryScreenManager.h"
#include "storageBoxScreenManager.h"
#include "skillbarManager.h"

#include "gamedata.h"
#include "options.h"


class UserScreen
{
	public:
		void (*dispFunction)(void);
		void (*manageFunction)(Event&);
};

UserScreen screenCharacter;
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


/** FONCTIONS DE GESTION DE LA PARTIE **/

void mainLoop()
{
    screenCharacter.dispFunction = Disp_Personnage;
    screenCharacter.manageFunction = nullptr;

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
    gui::Window inventoryWindow("gui/inventory.xml", Jeu.App);
    gui::Window storageBoxWindow("gui/storage-box.xml", Jeu.App);
    gui::Window ingameSkillbar("gui/ingame-skillbar.xml", Jeu.App);

    gui::TextWidget placeName;
    placeName.setCenterCoordinates(Options.ScreenW / 2, 120);
    placeName.setTextFont(gui::style::fontFromString("dayroman"), 30);
    placeName.setTextOutline(Color(0, 0, 0), 1);
    placeName.setTextColor("normal", Color(192, 0, 0));

    ingameToolbar.startWindow(Jeu.App);
    loadingWindow.startWindow(Jeu.App);
    inventoryWindow.startWindow(Jeu.App);
    storageBoxWindow.startWindow(Jeu.App);
    ingameSkillbar.startWindow(Jeu.App);

    options::initLoadGameWindow(loadGameWindow);
    options::initOptionsWindow(optionsWindow);

    tools::signals::addSignal("main-menu");
    tools::signals::addSignal("main-menu:disable-load-game");

    View worldView(FloatRect(0, 0, Options.ScreenW, Options.ScreenH));
    worldView.setViewport(sf::FloatRect(0, 0, 1, 1));

    Individu_Unique cursor;
    cursor.Type = "intern";
    cursor.size.circle(tools::math::Vector2d(0, 0), 5);
    cursor.interactionField.circle(tools::math::Vector2d(0, 0), 5);

    Individu_Unique screen;
    screen.Type = "intern";
    screen.interactionField.circle(tools::math::Vector2d(0, 0), Options.ScreenW/2);

    Individu* underCursor = nullptr;
    Coffre* storageBoxUnderCursor = nullptr;
    int selectedStorageBox = -1;
    bool cursorIsInWorld = false;
    bool move = false;
    string tooltip;

    enum LeftScreens { None, Inventory };
    LeftScreens currentLeftScreen = LeftScreens::None;

    enum BottomScreens { NoBottomScreen, StorageBox };
    BottomScreens currentBottomScreen = BottomScreens::NoBottomScreen;

	while (true)
	{
        //1. Events & Signals

        Event event;
        while (Jeu.App.pollEvent(event))
        {
            if (event.type == Event::MouseButtonPressed && cursorIsInWorld)
            {
                //Will force the player to stop the current activity (if possible) and adopt a new one immediately
                move = true;

                //Retrieve the current individual under the cursor
                if (underCursor != nullptr) gamedata::player()->selectedIndividual = underCursor->Id;

                //Retrieve the current storage box under the cursor
                if (storageBoxUnderCursor != nullptr && event.mouseButton.button == Mouse::Button::Left)
                    selectedStorageBox = storageBoxUnderCursor->Id;
                else
                {
                    selectedStorageBox = -1;

                    if (Partie.CoffreOuvert != nullptr)
                    {
                        Partie.CoffreOuvert->close();
                        currentBottomScreen = BottomScreens::NoBottomScreen;
                        Partie.CoffreOuvert = nullptr;
                    }
                }
            }

            if (event.type == Event::MouseButtonReleased && cursorIsInWorld)
                gamedata::player()->selectedIndividual = -1;

            ingameToolbar.manage(Jeu.App, event);
            ingameSkillbar.manage(Jeu.App, event);

            switch (currentLeftScreen)
            {
                case LeftScreens::Inventory :
                    manageInventoryScreen(inventoryWindow, Jeu.App, event);
                default:
                    break;
            }

            switch (currentBottomScreen)
            {
                case BottomScreens::StorageBox :
                    manageStorageBoxScreen(storageBoxWindow, Jeu.App, event);
                default:
                    break;
            }

            if (currentUserScreen != nullptr && currentUserScreen->manageFunction != nullptr)
                currentUserScreen->manageFunction(event);

            if (event.type == Event::Closed || (event.type == Event::KeyReleased && Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
                tools::signals::addSignal("ask-exit");

            if (event.type == Event::KeyPressed || event.type == Event::KeyReleased)
            {
                if (Partie.CoffreOuvert != nullptr)
                {
                    Partie.CoffreOuvert->close();
                    currentBottomScreen = BottomScreens::NoBottomScreen;
                    Partie.CoffreOuvert = nullptr;
                }
                selectedStorageBox = -1;
            }
        }

        ingameSkillbar.checkKeyboardState();

        worldView.reset(FloatRect(0, 0, Options.ScreenW, Options.ScreenH - 106));
        worldView.setViewport(FloatRect(0, 50.f/(float)Options.ScreenH, 1, (float)(Options.ScreenH-106)/(float)Options.ScreenH));

        if (currentLeftScreen != LeftScreens::None)
        {
            worldView.reset(FloatRect(0, 0, Options.ScreenW/2.f, Options.ScreenH - 106));
            worldView.setViewport(sf::FloatRect(0.5f, 50.f/(float)Options.ScreenH, 0.5f, (float)(Options.ScreenH-106)/(float)Options.ScreenH));
        }
        if (currentBottomScreen != BottomScreens::NoBottomScreen)
        {
            worldView.reset(FloatRect(0, 0, Options.ScreenW, Options.ScreenH - 306));
            worldView.setViewport(sf::FloatRect(0, 50.f/(float)Options.ScreenH, 1, (float)(Options.ScreenH - 306)/(float)Options.ScreenH));
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
                gamedata::loadFromXML(tools::filesystem::dataDirectory(), "chapitre/devilsai.xml");
                managementActivated = true;
            }

            if (signal.first == "start-game-tutorial")
            {
                loadingWindow.display(Jeu.App);
                Jeu.App.display();
                gamedata::loadFromXML(tools::filesystem::dataDirectory(), "chapitre/tutorial.xml");
                managementActivated = true;
            }
            if (signal.first == "change-player-name") {
                gamedata::setPlayerName(signal.second);
            }
            if (signal.first == "load-game")
            {
                loadGameWindow.manage(Jeu.App);
            }

            if (signal.first == "delete-game")
            {
                options::deleteSavedGamePack(signal.second);
                options::initLoadGameWindow(loadGameWindow);
            }

            if (signal.first == "start-loaded-game")
            {
                Load_Partie(signal.second);
                managementActivated = true;
            }

            if (signal.first == "options")
            {
                optionsWindow.manage(Jeu.App);
            }

            if (signal.first.find("option-change") != string::npos)
            {
                options::changeOption(signal.first, signal.second);
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
                if (currentLeftScreen == LeftScreens::Inventory)
                    currentLeftScreen = LeftScreens::None;
                else
                    currentLeftScreen = LeftScreens::Inventory;
            }

            if (signal.first == "screen-skills")
            {
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
                placeName.setAllText(tools::textManager::getText("places", signal.second));
                placeName.updateTextPosition();
            }

            if (signal.first == "player-skill") {
                gamedata::player()->Set_Activite(signal.second);
            }
            if (signal.first == "add-tooltip") {
                tooltip = signal.second;
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

        worldView.setCenter((int)gamedata::player()->position().x, (int)gamedata::player()->position().y - 100 * (currentBottomScreen != NoBottomScreen));

        if (managementActivated)
            gamedata::currentWorld()->GestionElements(worldView);

        //Screen position update
        worldView.setCenter((int)gamedata::player()->position().x, (int)gamedata::player()->position().y - 100 * (currentBottomScreen != NoBottomScreen));
        screen.move(-screen.position().x + gamedata::player()->position().x,
                    -screen.position().y + gamedata::player()->position().y);

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
        underCursor = nullptr;
        storageBoxUnderCursor = nullptr;

        int Resultat = COLL_OK;

        while(cursorIsInWorld && Resultat == COLL_OK)
        {
            Resultat = gamedata::currentWorld()->browseCollisionList(&cursor);

            if (Resultat == COLL_PRIM_MVT)
            {
                underCursor = dynamic_cast<Individu*>(gamedata::currentWorld()->getCurrentCollider());
                break;
            }
            if (Resultat == COLL_INTER)
            {
                storageBoxUnderCursor = dynamic_cast<Coffre*>(gamedata::currentWorld()->getCurrentCollider());
                break;
            }
        }

        if (gamedata::player()->selectedIndividual != -1 && gamedata::findElement(gamedata::player()->selectedIndividual) == nullptr)
            gamedata::player()->selectedIndividual = -1;
        if (selectedStorageBox != -1 && gamedata::findElement(selectedStorageBox) == nullptr)
            selectedStorageBox = -1;

        if (selectedStorageBox != -1)
        {
            Element_Carte* c = gamedata::findElement(selectedStorageBox);
            if (tools::math::intersection(gamedata::player()->interactionField, c->size))
            {
                gamedata::player()->stopAutomoving();
                Partie.CoffreOuvert = dynamic_cast<Coffre*>(c);
                currentBottomScreen = BottomScreens::StorageBox;
                currentLeftScreen = LeftScreens::None;
            }
            else
            {
                if (Partie.CoffreOuvert == nullptr)
                    gamedata::player()->automove(c->position());
                else
                {
                    Partie.CoffreOuvert->close();
                    currentBottomScreen = BottomScreens::NoBottomScreen;
                    Partie.CoffreOuvert = nullptr;
                    selectedStorageBox = -1;
                }
            }
        }

        if (cursorIsInWorld)
        {
            if (move)
            {
                move = false;
                gamedata::player()->Temps = 1;
            }

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                if (gamedata::player()->selectedIndividual == -1)
                {
                    gamedata::player()->automove(cursor.position());
                    gamedata::player()->stopHunting();
                }
                else if (gamedata::player()->selectedIndividual != gamedata::player()->Id)
                    gamedata::player()->hunt(gamedata::player()->selectedIndividual, ingameSkillbar.widget("left-click")->value());
            }
            if (Mouse::isButtonPressed(Mouse::Right))
            {
                if (gamedata::player()->selectedIndividual == -1)
                {
                    gamedata::player()->updateAngle(cursor.position());
                    gamedata::player()->Set_Activite(ingameSkillbar.widget("right-click")->value());
                }
                else if (gamedata::player()->selectedIndividual != gamedata::player()->Id)
                    gamedata::player()->hunt(underCursor->Id, ingameSkillbar.widget("right-click")->value());
            }

        }

        //3. Display

        Jeu.App.clear();
        Jeu.App.setView(worldView);

        gamedata::currentWorld()->displayBackground(Jeu.App);
        gamedata::currentWorld()->display(Jeu.App);

        if (underCursor != nullptr) underCursor->displayLifeGauge();

        if (storageBoxUnderCursor != nullptr) storageBoxUnderCursor->highlight(Jeu.App);

        if (!tooltip.empty())
        {
            int x = Jeu.App.mapPixelToCoords(Mouse::getPosition(Jeu.App), worldView).x;
            int y = Jeu.App.mapPixelToCoords(Mouse::getPosition(Jeu.App), worldView).y;
            Disp_TexteCentre(tools::textManager::getText("places", tooltip), x, y + 25, Color(255, 255, 255, 255), 15.f);
        }
        tooltip.clear();

        if (Keyboard::isKeyPressed(Keyboard::LAlt))
        {
            gamedata::currentWorld()->resetCollisionManager();
            for (int Resultat = COLL_OK ; Resultat != COLL_END ; Resultat = gamedata::currentWorld()->browseCollisionList(&screen))
            {
                if (Resultat == COLL_INTER)
                {
                    Coffre* c = dynamic_cast<Coffre*>(gamedata::currentWorld()->getCurrentCollider());
                    if (c != nullptr) c->highlight(Jeu.App);
                }
            }
        }

        if (gamedata::player()->ElementInteraction != -1)
        {
            Element_Carte* elem = gamedata::findElement(gamedata::player()->ElementInteraction);
            Individu* ind = dynamic_cast<Individu*>(elem);
            if (ind != nullptr && ind != underCursor) ind->displayLifeGauge();
        }

        Jeu.App.setView(Jeu.App.getDefaultView());

        if (!Partie.ModeCinematiques)
        {
            Disp_Menu();
            Disp_JaugesVie();
            Disp_Consoles();
        }

        ingameToolbar.display(Jeu.App);
        displaySkillbar(ingameSkillbar, Jeu.App);

        switch (currentLeftScreen)
        {
            case LeftScreens::Inventory :
                displayInventoryScreen(inventoryWindow, Jeu.App);
            default:
                break;
        }

        switch (currentBottomScreen)
        {
            case BottomScreens::StorageBox :
                displayStorageBoxScreen(storageBoxWindow, Jeu.App);
            default:
                break;
        }

        if (currentUserScreen != NULL)
            currentUserScreen->dispFunction();

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
			gamedata::loadFromXML(tools::filesystem::dataDirectory(), "chapitre/devilsai.xml");
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
	Partie.listDialogs.clear();
	currentUserScreen = nullptr;
	Partie.journal.entries.clear();

	SupprimerLignesConsoles();

    gamedata::clear();
}
