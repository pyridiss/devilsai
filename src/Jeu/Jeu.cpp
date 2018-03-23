
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
#include "tools/math.h"
#include "textManager/textManager.h"

#include "gui/window.h"
#include "gui/textWidget.h"

#include "imageManager/imageManager.h"

#include "musicManager/musicManager.h"

#include "Bibliotheque/Bibliotheque.h"
#include "../ElementsCarte/ElementsCarte.h"
#include "Jeu.h"

#include "skillbarManager.h"
#include "devilsai-gui/conversation.h"
#include "devilsai-gui/inventory.h"
#include "devilsai-gui/journal.h"
#include "devilsai-gui/console.h"
#include "devilsai-gui/storageBox.h"

#include "devilsai-resources/Carte.h"
#include "devilsai-resources/quests.h"
#include "gamedata.h"
#include "options.h"

enum GameState
{
    LeftPanelOpened = 1 << 0,
    ShowCharacter   = (1 << 0) + (1 << 1),
    ShowInventory   = (1 << 0) + (1 << 2),
    ShowJournal     = (1 << 0) + (1 << 3),
    ShowSkills      = (1 << 0) + (1 << 4),
    LeftPanels      = (1 << 0) + (1 << 1) + (1 << 2) + (1 << 3) + (1 << 4),
    ShowConsole     = 1 << 5,
    ShowStorageBox  = 1 << 6,
    LeftButtonPressed  = 1 << 7,
    RightButtonPressed = 1 << 8
};

void mainLoop(RenderWindow& app)
{
    uint16_t state = 0;

    bool managementActivated = false;
    bool playerResting = false;

    gui::Window mainMenuWindow("gui/main-menu.xml", app);
    gui::Window ingameToolbar("gui/ingame-toolbar.xml", app);
    gui::Window inventoryWindow("gui/inventory.xml", app);
    gui::Window storageBoxWindow("gui/storage-box.xml", app);
    gui::Window ingameSkillbar("gui/ingame-skillbar.xml", app);
    gui::Window characterWindow("gui/character.xml", app);

    gui::Widget* fps = ingameToolbar.widget("fps");
    gui::Widget* tooltip = ingameToolbar.widget("tooltip");
    gui::Widget* playerDescription = characterWindow.widget("text");

    initConsole(app);
    initConversation(app);
    initJournal(app);

    tools::signals::addSignal("main-menu:disable-load-game");

    options::initLoadGameWindow(mainMenuWindow);
    options::initOptionsWindow(mainMenuWindow);

    tools::signals::addSignal("main-menu");

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
    WearableItem selectedObject;
    int selectedStorageBox = -1;
    bool inGame = false;
    bool loadingResources = false;
    bool cursorIsInWorld = false;
    bool showTooltip = false;
    bool cinematicMode = false;

    if (options::option<bool>(tools::math::sdbm_hash("show-console")))
        state |= ShowConsole;

    auto closeStorageBox = [&]()
    {
        selectedStorageBox = -1;
        state &= ~ShowStorageBox;

        if (openStorageBox != nullptr)
        {
            openStorageBox->close();
            openStorageBox = nullptr;
        }
    };

    musicManager::playMusic(mainMenuWindow.music);

	while (true)
	{
        //1. Events & Signals

        if (loadingResources) imageManager::lockGLMutex(1);

        Event event;
        while (app.pollEvent(event))
        {
            // Common window management

            if (event.type == Event::MouseMoved)
                gui::updateMousePosition(app);

            if (event.type == Event::Resized)
                app.setView(View(FloatRect(0, 0, app.getSize().x, app.getSize().y)));

            if (event.type == Event::Closed || (event.type == Event::KeyReleased && Keyboard::isKeyPressed(Keyboard::F4) && Keyboard::isKeyPressed(Keyboard::LAlt)))
                tools::signals::addSignal("exit");

            // Gui windows

            if (inGame)
            {
                ingameToolbar.manage(app, event);
                ingameSkillbar.manage(app, event);
                manageConversation(app, event);

                if ((state & ShowInventory) == ShowInventory)
                    manageInventoryScreen(inventoryWindow, app, event, selectedObject);
                if ((state & ShowJournal) == ShowJournal)
                    manageJournal(app, event);
                if ((state & ShowStorageBox) == ShowStorageBox)
                    manageStorageBoxScreen(storageBoxWindow, app, event, openStorageBox);
                if ((state & ShowConsole) == ShowConsole && (state & LeftPanelOpened) == 0 && (state & ShowStorageBox) == 0)
                    manageConsole(app, event);
            }
            else
            {
                mainMenuWindow.manage(app, event);
            }

            // In-game events

            if (event.type == Event::MouseButtonPressed && cursorIsInWorld && !cinematicMode)
            {
                if (event.mouseButton.button == Mouse::Button::Left)
                    state |= LeftButtonPressed;
                if (event.mouseButton.button == Mouse::Button::Right)
                    state |= RightButtonPressed;

                //Retrieve the current individual under the cursor
                if (underCursor != nullptr) gamedata::player()->selectedIndividual = underCursor;

                //Retrieve the current storage box under the cursor
                if (storageBoxUnderCursor != nullptr && event.mouseButton.button == Mouse::Button::Left)
                    selectedStorageBox = storageBoxUnderCursor->Id;
                else closeStorageBox();
            }

            if (event.type == Event::MouseButtonReleased)
                state &= ~(LeftButtonPressed | RightButtonPressed);

            if (event.type == Event::KeyPressed || event.type == Event::KeyReleased)
                closeStorageBox();
        }

        if (inGame)
        {
            ingameToolbar.checkTriggers();
            ingameSkillbar.checkTriggers();
            characterWindow.checkTriggers();
            manageConversation(app);
        }
        else
        {
            mainMenuWindow.checkTriggers();
        }

        worldView.reset(FloatRect(0, 0, app.getSize().x, app.getSize().y - 106));
        worldView.setViewport(FloatRect(0, 50.f/(float)app.getSize().y, 1, (float)(app.getSize().y-106)/(float)app.getSize().y));

        if (loadingResources) imageManager::unlockGLMutex(1);

        if ((state & LeftPanelOpened) == LeftPanelOpened)
        {
            worldView.reset(FloatRect(0, 0, app.getSize().x/2.f, app.getSize().y - 106));
            worldView.setViewport(sf::FloatRect(0.5f, 50.f/(float)app.getSize().y, 0.5f, (float)(app.getSize().y-106)/(float)app.getSize().y));
        }
        if ((state & ShowStorageBox) == ShowStorageBox)
        {
            worldView.reset(FloatRect(0, 0, app.getSize().x, app.getSize().y - 306));
            worldView.setViewport(sf::FloatRect(0, 50.f/(float)app.getSize().y, 1, (float)(app.getSize().y - 306)/(float)app.getSize().y));
        }

        tools::signals::Signal signal = tools::signals::getNextSignal();
        while (signal.first != "")
        {
            if (signal.first == "error-occured") {
                //TODO: Do something here
            }
            if (signal.first == "main-menu")
            {
                inGame = false;
                managementActivated = false;
                musicManager::playMusic(mainMenuWindow.music);
            }
            if (signal.first == "pause-game") {
                managementActivated = false;
            }
            if (signal.first == "resume-game") {
                managementActivated = true;
            }
            if (signal.first == "savegame") {
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
            if (signal.first == "player-dead") {
                managementActivated = false;
            }
            if (signal.first == "clear-gamedata") {
                Clean_Partie();
            }
            if (signal.first == "start-new-game") {
                gamedata::loadGameDataFileAsync(tools::filesystem::dataDirectory(), signal.second);
                loadingResources = true;
            }
            if (signal.first == "game-started") {
                inGame = true;
                managementActivated = true;
            }
            if (signal.first == "change-player-name") {
                gamedata::setPlayerName(signal.second);
            }

            if (signal.first == "delete-game")
            {
                options::deleteSavedGamePack(signal.second);
                options::initLoadGameWindow(mainMenuWindow);
            }

            if (signal.first == "start-loaded-game") {
                Load_Partie(signal.second);
                loadingResources = true;
            }
            if (signal.first.find("option-change") != string::npos)
            {
                options::changeOption(signal.first, signal.second);
            }
            if (signal.first == "screen-character") {
                state = (state & ~LeftPanels) | (((state & ShowCharacter) == ShowCharacter) ? 0 : ShowCharacter);
            }
            if (signal.first == "screen-equipment") {
                state = (state & ~LeftPanels) | (((state & ShowInventory) == ShowInventory) ? 0 : ShowInventory);
            }
            if (signal.first == "screen-skills") {
                state = (state & ~LeftPanels) | (((state & ShowSkills) == ShowSkills) ? 0 : ShowSkills);
            }
            if (signal.first == "screen-journal") {
                state = (state & ~LeftPanels) | (((state & ShowJournal) == ShowJournal) ? 0 : ShowJournal);
                tools::signals::addSignal("ingame-toolbar:remove-warn-journal");
            }
            if (signal.first == "exit")
            {
                return;
            }
            if (signal.first == "player-skill") {
                gamedata::player()->Set_Activite(signal.second);
            }
            if (signal.first == "add-tooltip") {
                gui::optionType o;
                o.set<textManager::PlainText>(textManager::getText("places", signal.second));
                tooltip->setValue(o);
                showTooltip = true;
            }
            if (signal.first == "enable-cinematic-mode") {
                cinematicMode = true;
            }
            if (signal.first == "disable-cinematic-mode") {
                cinematicMode = false;
            }

            tools::signals::removeSignal();
            signal = tools::signals::getNextSignal();
        }

        if (!inGame)
        {
            managementActivated = false;

            if (loadingResources) imageManager::lockGLMutex(2);

            mainMenuWindow.display(app);
            tools::timeManager::frameDone();
            app.display();

            if (loadingResources) imageManager::unlockGLMutex(2);

            musicManager::manageRunningMusics();

            continue;
        }

        //2. Management

        worldView.setCenter((int)gamedata::player()->position().x, (int)gamedata::player()->position().y - 100 * ((state & ShowStorageBox) == ShowStorageBox));

        if (managementActivated)
            gamedata::currentWorld()->GestionElements(worldView);

        //Screen position update
        worldView.setCenter((int)gamedata::player()->position().x, (int)gamedata::player()->position().y - 100 * ((state & ShowStorageBox) == ShowStorageBox));
        screen.move(-screen.position().x + gamedata::player()->position().x,
                    -screen.position().y + gamedata::player()->position().y);

        //Mouse cursor
        cursor.move(-cursor.position().x + app.mapPixelToCoords(gui::mousePosition(), worldView).x,
                    -cursor.position().y + app.mapPixelToCoords(gui::mousePosition(), worldView).y);

        if (gui::mousePosition().x >= app.getSize().x * worldView.getViewport().left &&
            gui::mousePosition().y >= app.getSize().y * worldView.getViewport().top &&
            gui::mousePosition().x <= app.getSize().x * (worldView.getViewport().left + worldView.getViewport().width) &&
            gui::mousePosition().y <= app.getSize().y * (worldView.getViewport().top + worldView.getViewport().height))
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
                state |= ShowStorageBox;
                state &= ~LeftPanels;
            }
            else
            {
                if (openStorageBox == nullptr)
                    gamedata::player()->automove(c->position());
                else closeStorageBox();
            }
        }

        if (!cinematicMode && cursorIsInWorld)
        {
            if ((state & LeftButtonPressed) == LeftButtonPressed)
            {
                if (gamedata::player()->selectedIndividual == nullptr)
                {
                    gamedata::player()->automove(cursor.position());
                    gamedata::player()->stopHunting();
                }
                else if (gamedata::player()->selectedIndividual != gamedata::player())
                    gamedata::player()->hunt(gamedata::player()->selectedIndividual, ingameSkillbar.widget("left-click")->embeddedData<string>("value"), false);
            }
            else if ((state & RightButtonPressed) == RightButtonPressed)
            {
                if (gamedata::player()->selectedIndividual == nullptr)
                {
                    gamedata::player()->updateAngle(cursor.position());
                    gamedata::player()->Set_Activite(ingameSkillbar.widget("right-click")->embeddedData<string>("value"));
                }
                else if (gamedata::player()->selectedIndividual != gamedata::player())
                    gamedata::player()->hunt(gamedata::player()->selectedIndividual, ingameSkillbar.widget("right-click")->embeddedData<string>("value"), true);
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
            int x = gui::mousePosition().x + 10;
            int y = gui::mousePosition().y + 10;
            tooltip->setPosition(x, y);
            tooltip->show();
        }
        else tooltip->hide();

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
            gamedata::player()->Disp_JaugesVie(app);
        }

        displayConversation(app);

        if ((state & ShowInventory) == ShowInventory)
            displayInventoryScreen(inventoryWindow, app, selectedObject);
        if ((state & ShowJournal) == ShowJournal)
            displayJournal(app);
        if ((state & ShowCharacter) == ShowCharacter)
        {
            gui::optionType o;
            o.set<textManager::PlainText>(gamedata::player()->characterDescription());
            playerDescription->setValue(o);
            characterWindow.display(app);
        }
        if ((state & ShowStorageBox) == ShowStorageBox)
            displayStorageBoxScreen(storageBoxWindow, app, openStorageBox);
        if ((state & ShowConsole) == ShowConsole && (state & LeftPanelOpened) == 0 && (state & ShowStorageBox) == 0)
            displayConsole(app);

        gui::optionType o;
        o.set<textManager::PlainText>(tools::timeManager::getFPS());
        fps->setValue(o);

        ingameToolbar.display(app);
        displaySkillbar(ingameSkillbar, app);

        if (playerResting)
        {
            bool animationFinished = Disp_Repos(app);
            if (animationFinished)
            {
                playerResting = false;
                managementActivated = true;
            }
        }

		//2. GESTION DES MISSIONS

        resources::quests::manageQuests();

		//4. CHANGEMENTS DE LIEU

        gamedata::updateCurrentPlace();

        tools::timeManager::frameDone();
        app.display();
        musicManager::manageRunningMusics();
	}
}

void Clean_Partie()
{
    gamedata::clear();
}
