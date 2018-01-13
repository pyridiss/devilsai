
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

#include <lua.hpp>

#include "tools/debug.h"
#include "tools/filesystem.h"
#include "tools/timeManager.h"
#include "tools/signals.h"
#include "tools/textManager.h"

#include "gui/window.h"
#include "gui/textWidget.h"

#include "musicManager/musicManager.h"

#include "Bibliotheque/Bibliotheque.h"
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

void mainLoop(RenderWindow& app)
{
    screenCharacter.dispFunction = Disp_Personnage;
    screenCharacter.manageFunction = nullptr;

    screenJournal.dispFunction = displayJournal;
    screenJournal.manageFunction = nullptr;

	float ChangementLieu = 255;
	float SauvegardeEffectuee = 255;

    bool managementActivated = false;
    bool playerResting = false;

    gui::Window mainMenuWindow("gui/main-menu.xml");
    gui::Window newGameWindow("gui/new-game.xml");
    gui::Window loadGameWindow("gui/load-game.xml");
    gui::Window optionsWindow("gui/options.xml");
    gui::Window confirmExitGameWindow("gui/confirm-exit-game.xml");
    gui::Window ingameMenuWindow("gui/ingame-menu.xml");
    gui::Window ingameToolbar("gui/ingame-toolbar.xml");
    gui::Window loadingWindow("gui/loading.xml");
    gui::Window playerDeadWindow("gui/player-dead.xml");
    gui::Window inventoryWindow("gui/inventory.xml");
    gui::Window storageBoxWindow("gui/storage-box.xml");
    gui::Window ingameSkillbar("gui/ingame-skillbar.xml");

    gui::TextWidget placeName;
    placeName.setCenterCoordinates(Options.ScreenW / 2, 120);
    placeName.setTextFont(gui::style::fontFromString("dayroman"), 30);
    placeName.setTextOutline(Color(0, 0, 0), 1);
    placeName.setTextColor("normal", Color(192, 0, 0));

    gui::TextWidget tooltip;
    tooltip.setBackgroundShader("normal", "textBackground");
    tooltip.setTextFont(gui::style::fontFromString("liberation-bold"), 15);
    tooltip.setTextColor("normal", Color(255, 255, 255));
    ingameToolbar.startWindow(app);
    loadingWindow.startWindow(app);
    inventoryWindow.startWindow(app);
    storageBoxWindow.startWindow(app);
    ingameSkillbar.startWindow(app);


    options::initLoadGameWindow(loadGameWindow);
    options::initOptionsWindow(optionsWindow);

    tools::signals::addSignal("main-menu");
    tools::signals::addSignal("main-menu:disable-load-game");

    View worldView(FloatRect(0, 0, app.getSize().x, app.getSize().y));
    worldView.setViewport(sf::FloatRect(0, 0, 1, 1));

    Individu cursor;
    cursor.Type = "intern";
    cursor.size.circle(tools::math::Vector2d(0, 0), 5);

    Individu screen;
    screen.Type = "intern";
    screen.size.circle(tools::math::Vector2d(0, 0), app.getSize().x/2);

    Individu* underCursor = nullptr;
    Coffre* storageBoxUnderCursor = nullptr;
    Coffre* openStorageBox = nullptr;
    lua_State* selectedObject = nullptr;
    int selectedStorageBox = -1;
    bool cursorIsInWorld = false;
    bool move = false;
    bool showTooltip = false;

    enum LeftScreens { None, Inventory };
    LeftScreens currentLeftScreen = LeftScreens::None;

    enum BottomScreens { NoBottomScreen, StorageBox };
    BottomScreens currentBottomScreen = BottomScreens::NoBottomScreen;

	while (true)
	{
        //1. Events & Signals

        bool leftClick = false, rightClick = false;

        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::MouseButtonPressed && cursorIsInWorld)
            {
                if (event.mouseButton.button == Mouse::Button::Left)
                    leftClick = true;
                if (event.mouseButton.button == Mouse::Button::Right)
                    rightClick = true;

                //Will force the player to stop the current activity (if possible) and adopt a new one immediately
                move = true;

                //Retrieve the current individual under the cursor
                if (underCursor != nullptr) gamedata::player()->selectedIndividual = underCursor;

                //Retrieve the current storage box under the cursor
                if (storageBoxUnderCursor != nullptr && event.mouseButton.button == Mouse::Button::Left)
                    selectedStorageBox = storageBoxUnderCursor->Id;
                else
                {
                    selectedStorageBox = -1;

                    if (openStorageBox != nullptr)
                    {
                        openStorageBox->close();
                        currentBottomScreen = BottomScreens::NoBottomScreen;
                        openStorageBox = nullptr;
                    }
                }
            }

            ingameToolbar.manage(app, event);
            ingameSkillbar.manage(app, event);

            switch (currentLeftScreen)
            {
                case LeftScreens::Inventory :
                    manageInventoryScreen(inventoryWindow, app, event, selectedObject);
                default:
                    break;
            }

            switch (currentBottomScreen)
            {
                case BottomScreens::StorageBox :
                    manageStorageBoxScreen(storageBoxWindow, app, event, openStorageBox);
                default:
                    break;
            }

            if (currentUserScreen != nullptr && currentUserScreen->manageFunction != nullptr)
                currentUserScreen->manageFunction(event);

            if (event.type == Event::Closed || (event.type == Event::KeyReleased && Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
                tools::signals::addSignal("ask-exit");

            if (event.type == Event::KeyPressed || event.type == Event::KeyReleased)
            {
                if (openStorageBox != nullptr)
                {
                    openStorageBox->close();
                    currentBottomScreen = BottomScreens::NoBottomScreen;
                    openStorageBox = nullptr;
                }
                selectedStorageBox = -1;
            }
        }

        ingameSkillbar.checkKeyboardState();

        worldView.reset(FloatRect(0, 0, app.getSize().x, app.getSize().y - 106));
        worldView.setViewport(FloatRect(0, 50.f/(float)app.getSize().y, 1, (float)(app.getSize().y-106)/(float)app.getSize().y));

        if (currentLeftScreen != LeftScreens::None)
        {
            worldView.reset(FloatRect(0, 0, app.getSize().x/2.f, app.getSize().y - 106));
            worldView.setViewport(sf::FloatRect(0.5f, 50.f/(float)app.getSize().y, 0.5f, (float)(app.getSize().y-106)/(float)app.getSize().y));
        }
        if (currentBottomScreen != BottomScreens::NoBottomScreen)
        {
            worldView.reset(FloatRect(0, 0, app.getSize().x, app.getSize().y - 306));
            worldView.setViewport(sf::FloatRect(0, 50.f/(float)app.getSize().y, 1, (float)(app.getSize().y - 306)/(float)app.getSize().y));
        }

        tools::signals::Signal signal = tools::signals::getNextSignal();
        while (signal.first != "")
        {
            if (signal.first == "main-menu")
            {
                mainMenuWindow.manage(app);
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

            if (signal.first == "game-successfully-ended") {
                //TODO: Do something here
            }
            if (signal.first == "player-dead")
            {
                playerDeadWindow.manage(app);
                Clean_Partie();
            }

            if (signal.first == "new-game")
            {
                newGameWindow.manage(app);
            }

            if (signal.first == "start-new-game") {
                gamedata::clear();
                loadingWindow.display(app);
                app.display();
                gamedata::loadFromXML(tools::filesystem::dataDirectory(), signal.second);
                managementActivated = true;
            }
            if (signal.first == "change-player-name") {
                gamedata::setPlayerName(signal.second);
            }
            if (signal.first == "load-game")
            {
                loadGameWindow.manage(app);
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
                optionsWindow.manage(app);
            }

            if (signal.first.find("option-change") != string::npos)
            {
                options::changeOption(signal.first, signal.second);
            }

            if (signal.first == "ask-menu")
            {
                ingameMenuWindow.manage(app);
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
                confirmExitGameWindow.manage(app);
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
                tooltip.setSize(0, 0);
                tooltip.setAllText(tools::textManager::getText("places", signal.second));
                showTooltip = true;
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
        cursor.move(-cursor.position().x + app.mapPixelToCoords(Mouse::getPosition(app), worldView).x,
                    -cursor.position().y + app.mapPixelToCoords(Mouse::getPosition(app), worldView).y);

        if (Mouse::getPosition(app).x >= app.getSize().x * worldView.getViewport().left &&
            Mouse::getPosition(app).y >= app.getSize().y * worldView.getViewport().top &&
            Mouse::getPosition(app).x <= app.getSize().x * (worldView.getViewport().left + worldView.getViewport().width) &&
            Mouse::getPosition(app).y <= app.getSize().y * (worldView.getViewport().top + worldView.getViewport().height))
            cursorIsInWorld = true;
        else cursorIsInWorld = false;

        underCursor = nullptr;
        storageBoxUnderCursor = nullptr;

        if (cursorIsInWorld)
        {
            auto result = gamedata::currentWorld()->findFirstCollidingItem(&cursor, cursor.size, true);
            if (result.second == COLL_PRIM_MVT)
                underCursor = dynamic_cast<Individu*>(result.first);
            else if (result.second == COLL_INTER)
                storageBoxUnderCursor = dynamic_cast<Coffre*>(result.first);
        }

        if (selectedStorageBox != -1 && gamedata::findElement(selectedStorageBox) == nullptr)
            selectedStorageBox = -1;

        if (selectedStorageBox != -1)
        {
            Element_Carte* c = gamedata::findElement(selectedStorageBox);
            if (tools::math::intersection(gamedata::player()->interactionField, c->size))
            {
                gamedata::player()->stopAutomoving();
                openStorageBox = dynamic_cast<Coffre*>(c);
                currentBottomScreen = BottomScreens::StorageBox;
                currentLeftScreen = LeftScreens::None;
            }
            else
            {
                if (openStorageBox == nullptr)
                    gamedata::player()->automove(c->position());
                else
                {
                    openStorageBox->close();
                    currentBottomScreen = BottomScreens::NoBottomScreen;
                    openStorageBox = nullptr;
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

            if (Mouse::isButtonPressed(Mouse::Left) || leftClick)
            {
                if (gamedata::player()->selectedIndividual == nullptr)
                {
                    gamedata::player()->automove(cursor.position());
                    gamedata::player()->stopHunting();
                }
                else if (gamedata::player()->selectedIndividual != gamedata::player())
                    gamedata::player()->hunt(gamedata::player()->selectedIndividual, ingameSkillbar.widget("left-click")->value(), false);
            }
            else if (Mouse::isButtonPressed(Mouse::Right) || rightClick)
            {
                if (gamedata::player()->selectedIndividual == nullptr)
                {
                    gamedata::player()->updateAngle(cursor.position());
                    gamedata::player()->Set_Activite(ingameSkillbar.widget("right-click")->value());
                }
                else if (gamedata::player()->selectedIndividual != gamedata::player())
                    gamedata::player()->hunt(gamedata::player()->selectedIndividual, ingameSkillbar.widget("right-click")->value(), true);
            }
            else gamedata::player()->selectedIndividual = nullptr;

        }

        //3. Display

        app.clear();
        app.setView(worldView);

        gamedata::currentWorld()->displayBackground(app);
        gamedata::currentWorld()->display(app);

        if (underCursor != nullptr) underCursor->displayLifeGauge(app);

        if (storageBoxUnderCursor != nullptr) storageBoxUnderCursor->highlight(app);

        if (showTooltip)
        {
            int x = app.mapPixelToCoords(Mouse::getPosition(app), worldView).x + 10;
            int y = app.mapPixelToCoords(Mouse::getPosition(app), worldView).y + 10;
            tooltip.setTopLeftCoordinates(x, y);
            tooltip.updateTextPosition();
            tooltip.display(Jeu.App);
        }
        showTooltip = false;

        if (Keyboard::isKeyPressed(Keyboard::LAlt))
        {
            auto result = gamedata::currentWorld()->findAllCollidingItems(&screen, screen.size, false);
            for (auto& v : result)
            {
                if (v.second == COLL_INTER)
                {
                    Coffre* c = dynamic_cast<Coffre*>(v.first);
                    if (c != nullptr) c->highlight(app);
                }
            }
        }

        if (gamedata::player()->targetedItem() != nullptr)
        {
            Individu* ind = dynamic_cast<Individu*>(gamedata::player()->targetedItem());
            if (ind != nullptr && ind != underCursor) ind->displayLifeGauge(app);
        }

        app.setView(View(FloatRect(0, 0, app.getSize().x, app.getSize().y)));

        if (!options::option<bool>(tools::math::sdbm_hash("cinematic-mode")))
        {
            Disp_Menu(app);
            Disp_JaugesVie(app);
            Disp_Consoles();
        }

        ingameToolbar.display(app);
        displaySkillbar(ingameSkillbar, app);

        switch (currentLeftScreen)
        {
            case LeftScreens::Inventory :
                displayInventoryScreen(inventoryWindow, app, selectedObject);
            default:
                break;
        }

        switch (currentBottomScreen)
        {
            case BottomScreens::StorageBox :
                displayStorageBoxScreen(storageBoxWindow, app, openStorageBox);
            default:
                break;
        }

        if (currentUserScreen != NULL)
            currentUserScreen->dispFunction();

        Disp_FonduNoir(0, app);

        if (playerResting)
        {
            bool animationFinished = Disp_Repos(app);
            if (animationFinished)
            {
                playerResting = false;
                managementActivated = true;
            }
        }

        Disp_FPS(app);

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
				Disp_TexteCentre(tools::textManager::getText("devilsai", "SAUVEGARDE"), Options.ScreenW/2 + 1, 161, Color(0, 0, 0, SauvegardeEffectuee), 30., gui::style::fontFromString("dayroman"));
				Disp_TexteCentre(tools::textManager::getText("devilsai", "SAUVEGARDE"), Options.ScreenW/2, 160, Color(225, 0, 0, SauvegardeEffectuee), 30., gui::style::fontFromString("dayroman"));

			}
		}

        tools::timeManager::frameDone();
        app.display();
        musicManager::manageRunningMusics();
	}

    if (selectedObject != nullptr)
    {
        lua_close(selectedObject);
        selectedObject = nullptr;
    }
}

void Clean_Partie()
{
	currentUserScreen = nullptr;

	SupprimerLignesConsoles();

    gamedata::clear();
}
